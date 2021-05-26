#include <blah/images/font.h>
#include <blah/streams/filestream.h>
#include <blah/numerics/calc.h>
#include <blah/common.h>

using namespace Blah;

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "../third_party/stb_truetype.h"

namespace
{
	String get_font_name(stbtt_fontinfo* font, int nameId)
	{
		int length = 0;

		// get the name
		const u16* ptr = (const u16*)stbtt_GetFontNameStr(font, &length,
			STBTT_PLATFORM_ID_MICROSOFT,
			STBTT_MS_EID_UNICODE_BMP,
			STBTT_MS_LANG_ENGLISH,
			nameId);

		// we want the size in wide chars
		length /= 2;

		String str;
		if (length > 0)
			str.append_utf16(ptr, ptr + length, Calc::is_little_endian());
		return str;
	}
}

Font::Font()
{
	m_font = nullptr;
	m_data = nullptr;
	m_ascent = 0;
	m_descent = 0;
	m_line_gap = 0;
	m_valid = false;
}

Font::Font(Stream& stream) : Font()
{
	load(stream);
}

Font::Font(const FilePath& path) : Font()
{
	FileStream fs(path, FileMode::OpenRead);
	if (fs.is_readable())
		load(fs);
}

Font::Font(Font&& src) noexcept
{
	m_font = src.m_font;
	m_data = src.m_data;
	m_family_name = src.m_family_name;
	m_style_name = src.m_style_name;
	m_ascent = src.m_ascent;
	m_descent = src.m_descent;
	m_line_gap = src.m_line_gap;
	m_valid = src.m_valid;

	src.m_family_name.clear();
	src.m_style_name.clear();
	src.m_valid = false;
	src.m_font = nullptr;
	src.m_data = nullptr;
}

Font& Font::operator=(Font&& src) noexcept
{
	m_font = src.m_font;
	m_data = src.m_data;
	m_family_name = src.m_family_name;
	m_style_name = src.m_style_name;
	m_ascent = src.m_ascent;
	m_descent = src.m_descent;
	m_line_gap = src.m_line_gap;
	m_valid = src.m_valid;

	src.m_family_name.clear();
	src.m_style_name.clear();
	src.m_valid = false;
	src.m_font = nullptr;
	src.m_data = nullptr;
	return *this;
}

Font::~Font()
{
	dispose();
}

void Font::load(Stream& stream)
{
	dispose();

	if (!stream.is_readable())
	{
		BLAH_ASSERT(false, "Unable to load a font as the Stream was not readable");
		return;
	}

	// create data buffer
	auto size = stream.length();
	m_data = new unsigned char[size];
	stream.read(m_data, size);

	// init font
	m_font = new stbtt_fontinfo();
	auto fn = (stbtt_fontinfo*)m_font;
	stbtt_InitFont(fn, m_data, 0);
	m_family_name = get_font_name(fn, 1);
	m_style_name = get_font_name(fn, 2);
	
	// properties
	stbtt_GetFontVMetrics(fn, &m_ascent, &m_descent, &m_line_gap);
	m_valid = true;
}

void Font::dispose()
{
	delete (stbtt_fontinfo*)m_font;
	delete[] m_data;
	m_font = nullptr;
	m_data = nullptr;
	m_family_name.dispose();
	m_style_name.dispose();
}

const String& Font::family_name() const
{
	return m_family_name;
}

const String& Font::style_name() const
{
	return m_style_name;
}

int Font::ascent() const
{
	return m_ascent;
}

int Font::descent() const
{
	return m_descent;
}

int Font::line_gap() const
{
	return m_line_gap;
}

int Font::height() const
{
	return m_ascent - m_descent;
}

int Font::line_height() const
{
	return m_ascent - m_descent + m_line_gap;
}

int Font::get_glyph(Codepoint codepoint) const
{
	if (!m_font)
		return 0;
	return stbtt_FindGlyphIndex((stbtt_fontinfo*)m_font, codepoint);
}

float Font::get_scale(float size) const
{
	if (!m_font)
		return 0;
	
	return stbtt_ScaleForMappingEmToPixels((stbtt_fontinfo*)m_font, size);
}

float Font::get_kerning(int glyph1, int glyph2, float scale) const
{
	if (!m_font)
		return 0;
	return stbtt_GetGlyphKernAdvance((stbtt_fontinfo*)m_font, glyph1, glyph2) * scale;
}

Font::Character Font::get_character(int glyph, float scale) const
{
	Character ch;

	if (!m_font)
		return ch;

	int advance, offsetX, x0, y0, x1, y1;

	stbtt_GetGlyphHMetrics((stbtt_fontinfo*)m_font, glyph, &advance, &offsetX);
	stbtt_GetGlyphBitmapBox((stbtt_fontinfo*)m_font, glyph, scale, scale, &x0, &y0, &x1, &y1);

	int w = (x1 - x0);
	int h = (y1 - y0);

	// define character
	ch.glyph = glyph;
	ch.width = w;
	ch.height = h;
	ch.advance = advance * scale;
	ch.offset_x = offsetX * scale;
	ch.offset_y = (float)y0;
	ch.scale = scale;
	ch.has_glyph = (w > 0 && h > 0 && stbtt_IsGlyphEmpty((stbtt_fontinfo*)m_font, glyph) == 0);

	return ch;
}

bool Font::get_image(const Font::Character& ch, Color* pixels) const
{
	if (ch.has_glyph)
	{
		// we actually use the image buffer as our temporary buffer, and fill the pixels out backwards after
		// kinda weird but it works & saves creating more memory
		auto* src = (unsigned char*)pixels;
		stbtt_MakeGlyphBitmap((stbtt_fontinfo*)m_font, src, ch.width, ch.height, ch.width, ch.scale, ch.scale, ch.glyph);

		int len = ch.width * ch.height;
		for (int a = (len - 1) * 4, b = (len - 1); b >= 0; a -= 4, b -= 1)
		{
			src[a + 0] = src[b];
			src[a + 1] = src[b];
			src[a + 2] = src[b];
			src[a + 3] = src[b];
		}

		return true;
	}

	return false;
}

Image Font::get_image(const Font::Character& ch) const
{
	Image img(ch.width, ch.height);

	if (get_image(ch, img.pixels))
		return img;

	return Image();
}

bool Font::is_valid() const
{
	return m_valid;
}