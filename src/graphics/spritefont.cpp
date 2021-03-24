#include <blah/graphics/spritefont.h>
#include <blah/images/font.h>
#include <blah/images/packer.h>
#include <blah/core/common.h>

using namespace Blah;

SpriteFont::CharRange::CharRange()
	: from(0), to(0) {}
SpriteFont::CharRange::CharRange(Codepoint single)
	: from(single), to(single) {}
SpriteFont::CharRange::CharRange(Codepoint from, Codepoint to)
	: from(from), to(to) {}

const SpriteFont::CharSet SpriteFont::CharRange::ASCII = SpriteFont::CharSet({ CharRange(32, 128) });

SpriteFont::SpriteFont()
{
	size = 0;
	ascent = 0;
	descent = 0;
	line_gap = 0;
}

SpriteFont::SpriteFont(const FilePath& file, float size)
{
	rebuild(file, size, CharRange::ASCII);
}

SpriteFont::SpriteFont(const FilePath& file, float size, const CharSet& charset)
{
	rebuild(file, size, charset);
}

SpriteFont::SpriteFont(const Font& font, float size)
{
	rebuild(font, size, CharRange::ASCII);
}

SpriteFont::SpriteFont(const Font& font, float size, const CharSet& charset)
{
	rebuild(font, size, charset);
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

	Codepoint last = 0;
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

	Codepoint last = 0;
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

void SpriteFont::rebuild(const FilePath& file, float sz, const CharSet& charset)
{
	dispose();

	Font font(file);
	if (font.is_valid())
		rebuild(font, sz, charset);
}

void SpriteFont::rebuild(const Font& font, float size, const CharSet& charset)
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
	
	std::unordered_map<Codepoint, int> glyphs;
	Vector<Color> buffer;

	for (auto& range : charset)
	{
		auto from = range.from;
		auto to = range.to + 1;

		BLAH_ASSERT(to >= from, "Charset Range must be in pairs of [min,max]");

		for (auto i = from; i < to; i++)
		{
			auto glyph = font.get_glyph(i);
			if (glyph <= 0)
				continue;

			glyphs[i] = glyph;

			// add character
			auto ch = font.get_character(glyph, scale);
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
	}

	buffer.clear();
	packer.pack();

	for (auto& it : packer.pages)
		m_atlas.push_back(Texture::create(it));

	// add character subtextures
	for (auto& it : packer.entries())
		if (!it.empty)
			m_characters[(Codepoint)it.id].subtexture = Subtexture(m_atlas[it.page], it.packed, it.frame);

	// add kerning
	for (auto a = glyphs.begin(); a != glyphs.end(); a++)
		for (auto b = glyphs.begin(); b != glyphs.end(); b++)
		{
			auto kern = font.get_kerning(a->second, b->second, scale);
			if (kern != 0)
				set_kerning(a->first, b->first, kern);
		}
}

float SpriteFont::get_kerning(Codepoint codepoint0, Codepoint codepoint1) const
{
	u64 index = ((u64)codepoint0 << 32) | codepoint1;

	auto it = m_kerning.find(index);
	if (it != m_kerning.end())
		return it->second;
	return 0.0f;
}

void SpriteFont::set_kerning(Codepoint codepoint0, Codepoint codepoint1, float value)
{
	u64 index = ((u64)codepoint0 << 32) | codepoint1;

	if (value == 0)
	{
		m_kerning.erase(index);
	}
	else
	{
		m_kerning[index] = value;
	}
}

SpriteFont::Character& SpriteFont::get_character(Codepoint codepoint)
{
	return m_characters[codepoint];
}

const SpriteFont::Character& SpriteFont::get_character(Codepoint codepoint) const
{
	static const Character empty;
	auto it = m_characters.find(codepoint);
	if (it != m_characters.end())
		return it->second;
	return empty;
}

SpriteFont::Character& SpriteFont::operator[](Codepoint codepoint)
{
	return m_characters[codepoint];
}

const SpriteFont::Character& SpriteFont::operator[](Codepoint codepoint) const
{
	static const Character empty;
	auto it = m_characters.find(codepoint);
	if (it != m_characters.end())
		return it->second;
	return empty;
}
