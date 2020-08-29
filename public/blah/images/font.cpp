#include <blah/images/font.h>
#include <blah/streams/filestream.h>
#include <blah/math/calc.h>
#include <blah/log.h>

using namespace Blah;

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <blah/third_party/stb_truetype.h>

String GetName(stbtt_fontinfo* font, int nameId)
{
	int length = 0;

	// get the name
	const uint16_t* ptr = (const uint16_t*)stbtt_GetFontNameStr(font, &length,
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

Font::Font()
{
	font = nullptr;
	data = nullptr;
	ascent = 0;
	descent = 0;
	lineGap = 0;
	valid = false;
}

Font::Font(Stream& stream) : Font()
{
	Load(stream);
}

Font::Font(const char* path) : Font()
{
	FileStream fs(path, FileMode::Read);
	if (fs.is_readable())
		Load(fs);
}

Font::Font(Font&& src) noexcept
{
	font = src.font;
	data = src.data;
	familyName = src.familyName;
	styleName = src.styleName;
	ascent = src.ascent;
	descent = src.descent;
	lineGap = src.lineGap;
	valid = src.valid;

	src.familyName.clear();
	src.styleName.clear();
	src.valid = false;
	src.font = nullptr;
	src.data = nullptr;
}

Font& Font::operator=(Font&& src) noexcept
{
	font = src.font;
	data = src.data;
	familyName = src.familyName;
	styleName = src.styleName;
	ascent = src.ascent;
	descent = src.descent;
	lineGap = src.lineGap;
	valid = src.valid;

	src.familyName.clear();
	src.styleName.clear();
	src.valid = false;
	src.font = nullptr;
	src.data = nullptr;
	return *this;
}

Font::~Font()
{
	dispose();
}

void Font::Load(Stream& stream)
{
	dispose();

	if (!stream.is_readable())
	{
		BLAH_ERROR("Unable to load a font as the Stream was not readable");
		return;
	}

	// create data buffer
	auto size = stream.length();
	data = new unsigned char[size];
	stream.read(data, size);

	// init font
	font = new stbtt_fontinfo();
	auto fn = (stbtt_fontinfo*)font;
	stbtt_InitFont(fn, data, 0);
	familyName = GetName(fn, 1);
	styleName = GetName(fn, 2);
	
	// properties
	stbtt_GetFontVMetrics(fn, &ascent, &descent, &lineGap);
	valid = true;
}

void Font::dispose()
{
	delete (stbtt_fontinfo*)font;
	delete[] data;
	font = nullptr;
	data = nullptr;
	familyName.dispose();
	styleName.dispose();
}

const char* Font::FamilyName() const
{
	return familyName.cstr();
}

const char* Font::StyleName() const
{
	return styleName.cstr();
}

int Font::Ascent() const
{
	return ascent;
}

int Font::Descent() const
{
	return descent;
}

int Font::LineGap() const
{
	return lineGap;
}

int Font::Height() const
{
	return ascent - descent;
}

int Font::LineHeight() const
{
	return ascent - descent + lineGap;
}

int Font::GetGlyph(Codepoint codepoint) const
{
	if (font == nullptr)
		return 0;
	return stbtt_FindGlyphIndex((stbtt_fontinfo*)font, codepoint);
}

float Font::GetScale(float size) const
{
	if (font == nullptr)
		return 0;
	return stbtt_ScaleForPixelHeight((stbtt_fontinfo*)font, size);
}

float Font::GetKerning(int glyph1, int glyph2, float scale) const
{
	if (font == nullptr)
		return 0;
	return stbtt_GetGlyphKernAdvance((stbtt_fontinfo*)font, glyph1, glyph2) * scale;
}

Font::Char Font::GetCharacter(int glyph, float scale) const
{
	Char ch;

	if (font == nullptr)
		return ch;

	int advance, offsetX, x0, y0, x1, y1;

	stbtt_GetGlyphHMetrics((stbtt_fontinfo*)font, glyph, &advance, &offsetX);
	stbtt_GetGlyphBitmapBox((stbtt_fontinfo*)font, glyph, scale, scale, &x0, &y0, &x1, &y1);

	int w = (x1 - x0);
	int h = (y1 - y0);

	// define character
	ch.glyph = glyph;
	ch.width = w;
	ch.height = h;
	ch.advance = advance * scale;
	ch.offsetX = offsetX * scale;
	ch.offsetY = (float)y0;
	ch.scale = scale;
	ch.hasGlyph = (w > 0 && h > 0 && stbtt_IsGlyphEmpty((stbtt_fontinfo*)font, glyph) == 0);

	return ch;
}

bool Font::GetBitmap(const Font::Char& ch, Color* pixels) const
{
	if (ch.hasGlyph)
	{
		// we actually use the image buffer as our temporary buffer, and fill the pixels out backwards after
		// kinda weird but it works & saves creating more memory
		unsigned char* src = (unsigned char*)pixels;
		stbtt_MakeGlyphBitmap((stbtt_fontinfo*)font, src, ch.width, ch.height, ch.width, ch.scale, ch.scale, ch.glyph);

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

bool Font::IsValid() const
{
	return valid;
}