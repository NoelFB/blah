#include <blah/drawing/spritefont.h>
#include <blah/images/font.h>
#include <blah/images/packer.h>

using namespace Blah;

SpriteFont::SpriteFont()
{
	m_size = 0;
	m_ascent = 0;
	m_descent = 0;
	m_line_gap = 0;
}

const uint32_t ascii[]{ 32, 128, 0 };
const uint32_t* SpriteFont::ASCII = ascii;

SpriteFont::SpriteFont(const char* file, float size)
{
	build(file, size, ASCII);
}

SpriteFont::SpriteFont(const char* file, float size, const uint32_t* charset)
{
	build(file, size, charset);
}

SpriteFont::SpriteFont(const Font& font, float size)
{
	build(font, size, ASCII);
}

SpriteFont::SpriteFont(const Font& font, float size, const uint32_t* charset)
{
	build(font, size, charset);
}

SpriteFont::SpriteFont(SpriteFont&& src) noexcept
{
	m_name = src.m_name;
	m_size = src.m_size;
	m_ascent = src.m_ascent;
	m_descent = src.m_descent;
	m_line_gap = src.m_line_gap;
	m_characters = std::move(src.m_characters);
	m_kerning = std::move(src.m_kerning);
	m_atlas = std::move(src.m_atlas);
}

SpriteFont::~SpriteFont()
{
	dispose();
}

void SpriteFont::dispose()
{
	m_atlas.dispose();
	m_characters.clear();
	m_kerning.clear();
	m_name.dispose();
}

SpriteFont& SpriteFont::operator=(SpriteFont && src) noexcept
{
	m_name = src.m_name;
	m_size = src.m_size;
	m_ascent = src.m_ascent;
	m_descent = src.m_descent;
	m_line_gap = src.m_line_gap;
	m_characters = std::move(src.m_characters);
	m_kerning = std::move(src.m_kerning);
	m_atlas = std::move(src.m_atlas);
	return *this;
}

float SpriteFont::width_of(const String& text) const
{
	float width = 0;
	float lineWidth = 0;
	for (auto it = text.begin(); it != text.end(); it++)
	{
		if (*it == '\n')
			lineWidth = 0;

		// TODO: this doesn't account for Unicode values!
		uint32_t codepoint = *it;

		lineWidth += this->operator[](codepoint).advance;
		if (lineWidth > width)
			width = lineWidth;
	}

	return width;
}

float SpriteFont::width_of_line(const String& text, int start) const
{
	if (start < 0) return 0;
	if (start >= text.length()) return 0;

	float lineWidth = 0;
	for (auto it = text.begin() + start; it != text.end(); it++)
	{
		if (*it == '\n')
			return lineWidth;

		// TODO: this doesn't account for Unicode values!
		uint32_t codepoint = *it;

		lineWidth += this->operator[](codepoint).advance;
	}

	return lineWidth;
}

float SpriteFont::height_of(const String& text) const
{
	if (text.length() <= 0)
		return 0;

	float height = line_height();
	for (auto it = text.begin(); it != text.end(); it++)
	{
		if (*it == '\n')
			height += line_height();
	}

	return height - m_line_gap;
}

void SpriteFont::build(const char* file, float sz, const uint32_t* charset)
{
	dispose();

	Font font(file);
	if (font.IsValid())
		build(font, sz, charset);
}

void SpriteFont::build(const Font& font, float size, const uint32_t* charset)
{
	dispose();

	float scale = font.GetScale(size);

	m_name = font.FamilyName();
	m_ascent = font.Ascent() * scale;
	m_descent = font.Descent() * scale;
	m_line_gap = font.LineGap() * scale;
	m_size = size;

	Packer packer;
	packer.spacing = 0;
	packer.padding = 1;
	packer.max_size = 8192;
	packer.power_of_two = true;
	
	std::map<uint32_t, int> glyphs;
	List<Color> buffer;

	auto ranges = charset;
	while (*ranges != 0)
	{
		auto from = *ranges;
		auto to = *(ranges + 1);

		BLAH_ASSERT(to >= from, "Charset Range must be in pairs of [min,max]");

		for (auto i = from; i < to; i++)
		{
			auto glyph = font.GetGlyph(i);
			if (glyph <= 0)
				continue;

			glyphs[i] = glyph;

			// add character
			Font::Char ch = font.GetCharacter(glyph, scale);
			m_characters[i].advance = ch.advance;
			m_characters[i].offset = Vec2(ch.offsetX, ch.offsetY);

			// pack glyph
			if (ch.hasGlyph)
			{
				if (buffer.count() < ch.width * ch.height)
					buffer.expand(ch.width * ch.height - buffer.count());

				if (font.GetBitmap(ch, buffer.begin()))
					packer.add(i, ch.width, ch.height, buffer.begin());
			}
		}

		ranges += 2;
	}

	buffer.dispose();
	packer.pack();

	for (auto& it : packer.pages)
		m_atlas.add(Graphics::create_texture(it));

	// add character subtextures
	for (auto& it : packer.entries)
		if (!it.empty)
			m_characters[(uint32_t)it.id].subtexture = Subtexture(m_atlas[it.page], it.packed, it.frame);

	// add kerning
	for (auto a = glyphs.begin(); a != glyphs.end(); a++)
		for (auto b = glyphs.begin(); b != glyphs.end(); b++)
			set_kerning(a->first, b->first, font.GetKerning(a->second, b->second, scale));
}

float SpriteFont::get_kerning(uint32_t codepoint0, uint32_t codepoint1) const
{
	Tuple index;
	index.first = codepoint0;
	index.second = codepoint1;

	auto it = m_kerning.find(index);
	if (it != m_kerning.end())
		return it->second;
	return 0.0f;
}

void SpriteFont::set_kerning(uint32_t codepoint0, uint32_t codepoint1, float value)
{
	Tuple index;
	index.first = codepoint0;
	index.second = codepoint1;
	m_kerning[index] = value;
}


const SpriteFont::Character& SpriteFont::get_character(uint32_t codepoint) const
{
	static const Character empty;
	auto it = m_characters.find(codepoint);
	if (it != m_characters.end())
		return it->second;
	return empty;
}

const SpriteFont::Character& SpriteFont::operator[](uint32_t codepoint) const
{
	static const Character empty;
	auto it = m_characters.find(codepoint);
	if (it != m_characters.end())
		return it->second;
	return empty;
}