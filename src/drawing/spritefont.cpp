#include <blah/drawing/spritefont.h>
#include <blah/images/font.h>
#include <blah/images/packer.h>
#include <blah/core/log.h>

using namespace Blah;

SpriteFont::SpriteFont()
{
	size = 0;
	ascent = 0;
	descent = 0;
	line_gap = 0;
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
	name = src.name;
	size = src.size;
	ascent = src.ascent;
	descent = src.descent;
	line_gap = src.line_gap;
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
	m_atlas.clear();
	m_characters.clear();
	m_kerning.clear();
	name.dispose();
}

SpriteFont& SpriteFont::operator=(SpriteFont && src) noexcept
{
	name = src.name;
	size = src.size;
	ascent = src.ascent;
	descent = src.descent;
	line_gap = src.line_gap;
	m_characters = std::move(src.m_characters);
	m_kerning = std::move(src.m_kerning);
	m_atlas = std::move(src.m_atlas);
	return *this;
}

float SpriteFont::width_of(const String& text) const
{
	float width = 0;
	float line_width = 0;

	uint32_t last;
	for (int i = 0; i < text.length(); i ++)
	{
		if (text[i] == '\n')
		{
			line_width = 0;
			continue;
		}

		// get codepoint
		auto next = text.utf8_at(i);

		// increment length
		line_width += this->operator[](next).advance;
		
		// add kerning
		if (i > 0)
			line_width += get_kerning(last, next);

		if (line_width > width)
			width = line_width;

		// move to thext utf8 character
		i += text.utf8_length(i) - 1;
		last = next;
	}

	return width;
}

float SpriteFont::width_of_line(const String& text, int start) const
{
	if (start < 0) return 0;
	if (start >= text.length()) return 0;

	float width = 0;

	uint32_t last;
	for (int i = start; i < text.length(); i ++)
	{
		if (text[i] == '\n')
			return width;

		// get codepoint
		auto next = text.utf8_at(i);

		// increment length
		width += this->operator[](next).advance;
		
		// add kerning
		if (i > 0)
			width += get_kerning(last, next);

		// move to thext utf8 character
		i += text.utf8_length(i) - 1;

		last = next;
	}

	return width;
}

float SpriteFont::height_of(const String& text) const
{
	if (text.length() <= 0)
		return 0;

	float height = line_height();
	for (int i = 0; i < text.length(); i ++)
	{
		if (text[i] == '\n')
			height += line_height();
		i += text.utf8_length(i) - 1;
	}

	return height - line_gap;
}

void SpriteFont::build(const char* file, float sz, const uint32_t* charset)
{
	dispose();

	Font font(file);
	if (font.is_valid())
		build(font, sz, charset);
}

void SpriteFont::build(const Font& font, float size, const uint32_t* charset)
{
	dispose();

	float scale = font.get_scale(size);

	name = font.family_name();
	ascent = font.ascent() * scale;
	descent = font.descent() * scale;
	line_gap = font.line_gap() * scale;
	this->size = size;

	Packer packer;
	packer.spacing = 0;
	packer.padding = 1;
	packer.max_size = 8192;
	packer.power_of_two = true;
	
	std::unordered_map<uint32_t, int> glyphs;
	Vector<Color> buffer;

	auto ranges = charset;
	while (*ranges != 0)
	{
		auto from = *ranges;
		auto to = *(ranges + 1);

		BLAH_ASSERT(to >= from, "Charset Range must be in pairs of [min,max]");

		for (auto i = from; i < to; i++)
		{
			auto glyph = font.get_glyph(i);
			if (glyph <= 0)
				continue;

			glyphs[i] = glyph;

			// add character
			Font::Char ch = font.get_character(glyph, scale);
			m_characters[i].advance = ch.advance;
			m_characters[i].offset = Vec2(ch.offset_x, ch.offset_y);

			// pack glyph
			if (ch.has_glyph)
			{
				if (buffer.size() < ch.width * ch.height)
					buffer.resize(ch.width * ch.height);

				if (font.get_image(ch, buffer.data()))
					packer.add(i, ch.width, ch.height, buffer.data());
			}
		}

		ranges += 2;
	}

	buffer.clear();
	packer.pack();

	for (auto& it : packer.pages)
		m_atlas.push_back(Texture::create(it));

	// add character subtextures
	for (auto& it : packer.entries)
		if (!it.empty)
			m_characters[(uint32_t)it.id].subtexture = Subtexture(m_atlas[it.page], it.packed, it.frame);

	// add kerning
	for (auto a = glyphs.begin(); a != glyphs.end(); a++)
		for (auto b = glyphs.begin(); b != glyphs.end(); b++)
		{
			auto kern = font.get_kerning(a->second, b->second, scale);
			if (kern != 0)
				set_kerning(a->first, b->first, kern);
		}
}

float SpriteFont::get_kerning(uint32_t codepoint0, uint32_t codepoint1) const
{
	uint64_t index = ((uint64_t)codepoint0 << 32) | codepoint1;

	auto it = m_kerning.find(index);
	if (it != m_kerning.end())
		return it->second;
	return 0.0f;
}

void SpriteFont::set_kerning(uint32_t codepoint0, uint32_t codepoint1, float value)
{
	uint64_t index = ((uint64_t)codepoint0 << 32) | codepoint1;

	if (value == 0)
	{
		m_kerning.erase(index);
	}
	else
	{
		m_kerning[index] = value;
	}
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