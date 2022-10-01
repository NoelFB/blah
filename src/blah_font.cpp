#include <blah_font.h>
#include <blah_calc.h>

using namespace Blah;

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "third_party/stb_truetype.h"

namespace
{
	String blah_get_font_name(stbtt_fontinfo* font, int name_id)
	{
		int length = 0;

		// get the name
		const u16* ptr = (const u16*)stbtt_GetFontNameStr(font, &length,
			STBTT_PLATFORM_ID_MICROSOFT,
			STBTT_MS_EID_UNICODE_BMP,
			STBTT_MS_LANG_ENGLISH,
			name_id);

		// we want the size in wide chars
		length /= 2;

		String str;
		if (length > 0)
			str.append(ptr, ptr + length, Calc::is_little_endian());
		return str;
	}
}

FontRef Font::create(Stream& stream)
{
	if (!stream.is_readable())
	{
		Log::error("Unable to load a font as the Stream was not readable");
		return FontRef();
	}

	// read into buffer
	Vector<u8> buffer;
	buffer.reserve(stream.length());
	stream.read(buffer.data(), stream.length());

	// init font
	auto stbtt = Ref<stbtt_fontinfo>(new stbtt_fontinfo());
	auto fn = (stbtt_fontinfo*)stbtt.get();
	if (stbtt_InitFont(fn, buffer.data(), 0) == 0)
	{
		Log::error("Unable to parse Font file");
		return FontRef();
	}

	// setup
	auto font = FontRef(new Font());
	font->m_font = stbtt;
	font->m_buffer = std::move(buffer);
	font->m_family_name = blah_get_font_name(fn, 1);
	font->m_style_name = blah_get_font_name(fn, 2);
	stbtt_GetFontVMetrics(fn, &font->m_ascent, &font->m_descent, &font->m_line_gap);
	return font;
}

FontRef Font::create(const FilePath& path)
{
	FileStream fs(path, FileMode::OpenRead);
	if (fs.is_readable())
		return create(fs);
	return FontRef();
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
	return stbtt_FindGlyphIndex((stbtt_fontinfo*)m_font.get(), codepoint);
}

float Font::get_scale(float size) const
{
	if (!m_font)
		return 0;
	
	return stbtt_ScaleForMappingEmToPixels((stbtt_fontinfo*)m_font.get(), size);
}

float Font::get_kerning(int glyph1, int glyph2, float scale) const
{
	if (!m_font)
		return 0;
	return stbtt_GetGlyphKernAdvance((stbtt_fontinfo*)m_font.get(), glyph1, glyph2) * scale;
}

Font::Character Font::get_character(int glyph, float scale) const
{
	Character ch;

	if (!m_font)
		return ch;

	int advance, offsetX, x0, y0, x1, y1;

	stbtt_GetGlyphHMetrics((stbtt_fontinfo*)m_font.get(), glyph, &advance, &offsetX);
	stbtt_GetGlyphBitmapBox((stbtt_fontinfo*)m_font.get(), glyph, scale, scale, &x0, &y0, &x1, &y1);

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
	ch.has_glyph = (w > 0 && h > 0 && stbtt_IsGlyphEmpty((stbtt_fontinfo*)m_font.get(), glyph) == 0);

	return ch;
}

bool Font::get_image(const Font::Character& ch, Color* pixels) const
{
	if (ch.has_glyph)
	{
		// we actually use the image buffer as our temporary buffer, and fill the pixels out backwards after
		// kinda weird but it works & saves creating more memory
		auto* src = (unsigned char*)pixels;
		stbtt_MakeGlyphBitmap((stbtt_fontinfo*)m_font.get(), src, ch.width, ch.height, ch.width, ch.scale, ch.scale, ch.glyph);

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