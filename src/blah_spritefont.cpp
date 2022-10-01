#include <blah_spritefont.h>
#include <blah_packer.h>

using namespace Blah;

SpriteFont::CharRange::CharRange()
	: from(0), to(0) {}
SpriteFont::CharRange::CharRange(Codepoint single)
	: from(single), to(single) {}
SpriteFont::CharRange::CharRange(Codepoint from, Codepoint to)
	: from(from), to(to) {}

const SpriteFont::CharSet SpriteFont::CharRange::ASCII = SpriteFont::CharSet({ CharRange(32, 128) });

SpriteFont::SpriteFont(const FilePath& file, float size)
{
	rebuild(file, size, CharRange::ASCII);
}

SpriteFont::SpriteFont(const FilePath& file, float size, const CharSet& charset)
{
	rebuild(file, size, charset);
}

SpriteFont::SpriteFont(const FontRef& font, float size)
{
	rebuild(font, size, CharRange::ASCII);
}

SpriteFont::SpriteFont(const FontRef& font, float size, const CharSet& charset)
{
	rebuild(font, size, charset);
}

void SpriteFont::clear()
{
	m_atlas.clear();
	m_characters.clear();
	m_kerning.clear();
	name.clear();
}

float SpriteFont::width_of(const String& text) const
{
	float width = 0;
	float line_width = 0;

	Codepoint last = 0;

	Utf8 utf8(text.cstr());
	while (utf8.character)
	{
		if (utf8.character == '\n')
		{
			line_width = 0;
		}
		else
		{
			line_width += get_character(utf8.character).advance;
			if (last)
				line_width += get_kerning(last, utf8.character);
			if (line_width > width)
				width = line_width;
			last = utf8.character;
		}

		utf8.next();
	}

	return width;
}

float SpriteFont::width_of_line(const String& text, int start) const
{
	if (start < 0) return 0;
	if (start >= text.length()) return 0;

	float width = 0;
	Codepoint last = 0;

	Utf8 utf8(text.cstr());
	while (utf8.character)
	{
		if (utf8.character == '\n')
			break;

		width += get_character(utf8.character).advance;
		if (last)
			width += get_kerning(last, utf8.character);
		last = utf8.character;
		utf8.next();
	}

	return width;
}

float SpriteFont::height_of(const String& text) const
{
	if (text.length() <= 0)
		return 0;

	float height = line_height();

	Utf8 utf8(text.cstr());
	while (utf8.character)
	{
		if (utf8.character == '\n')
			height += line_height();
		utf8.next();
	}

	return height - line_gap;
}

void SpriteFont::rebuild(const FilePath& file, float sz, const CharSet& charset)
{
	clear();

	if (auto font = Font::create(file))
		rebuild(font, sz, charset);
}

void SpriteFont::rebuild(const FontRef& font, float size, const CharSet& charset)
{
	clear();
	if (!font)
		return;

	float scale = font->get_scale(size);

	name = font->family_name();
	ascent = font->ascent() * scale;
	descent = font->descent() * scale;
	line_gap = font->line_gap() * scale;
	this->size = size;

	Packer packer;
	packer.spacing = 0;
	packer.padding = 1;
	packer.max_size = 8192;
	packer.power_of_two = true;
	
	Vector<Color> buffer;

	for (auto& range : charset)
	{
		auto from = range.from;
		auto to = range.to + 1;

		BLAH_ASSERT(to >= from, "Charset Range must be in pairs of [min,max]");

		for (auto i = from; i < to; i++)
		{
			auto glyph = font->get_glyph(i);
			if (glyph <= 0)
				continue;

			// add character
			auto ch = font->get_character(glyph, scale);
			auto& sfch = get_character(i);
			sfch.codepoint = i;
			sfch.advance = ch.advance;
			sfch.offset = Vec2f(ch.offset_x, ch.offset_y);

			// pack glyph
			if (ch.has_glyph)
			{
				if (buffer.size() < ch.width * ch.height)
					buffer.resize(ch.width * ch.height);

				if (font->get_image(ch, buffer.data()))
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
			get_character((Codepoint)it.id).subtexture = Subtexture(m_atlas[it.page], it.packed, it.frame);

	// add kerning
	for (auto& a : m_characters)
	{
		for (auto& b : m_characters)
		{
			auto kerning_value = font->get_kerning(a.glyph, b.glyph, scale);
			if (kerning_value != 0)
				set_kerning(a.codepoint, b.codepoint, kerning_value);
		}
	}
}

namespace
{
	bool find_character_index(const Vector<SpriteFont::Character>& items, SpriteFont::Codepoint codepoint, int lower, int higher, int* index)
	{
		while (lower <= higher)
		{
			// found
			int mid = (higher + lower) / 2;
			if (items[mid].codepoint == codepoint)
			{
				*index = mid;
				return true;
			}

			// search upper / lower bounds
			if (items[mid].codepoint < codepoint)
				lower = mid + 1;
			else
				higher = mid - 1;
		}

		// not found
		*index = lower;
		return false;
	}

	bool find_kerning_index(const Vector<SpriteFont::Kerning>& items, SpriteFont::Codepoint a, SpriteFont::Codepoint b, int lower, int higher, int* index)
	{
		while (lower <= higher)
		{
			// found
			int mid = (higher + lower) / 2;
			if (items[mid].a == a && items[mid].b == b)
			{
				*index = mid;
				return true;
			}

			// search upper / lower bounds
			if (items[mid].a == a)
			{
				if (items[mid].b < b)
					lower = mid + 1;
				else
					higher = mid - 1;
			}
			else if (items[mid].a < a)
				lower = mid + 1;
			else
				higher = mid - 1;
		}

		// not found
		*index = lower;
		return false;
	}
}

float SpriteFont::get_kerning(Codepoint a, Codepoint b) const
{
	int index;
	if (find_kerning_index(m_kerning, a, b, 0, m_kerning.size() - 1, &index))
		return m_kerning[index].value;
	return 0;
}

void SpriteFont::set_kerning(Codepoint a, Codepoint b, float value)
{
	int index;
	if (find_kerning_index(m_kerning, a, b, 0, m_kerning.size() - 1, &index))
	{
		m_kerning[index].value = value;
	}
	else
	{
		m_kerning.expand();
		for (int i = m_kerning.size() - 1; i > index; i--)
			m_kerning[i] = std::move(m_kerning[i - 1]);
		m_kerning[index].a = a;
		m_kerning[index].b = b;
		m_kerning[index].value = value;
	}
}

SpriteFont::Character& SpriteFont::get_character(Codepoint codepoint)
{
	int index;
	if (find_character_index(m_characters, codepoint, 0, m_characters.size() - 1, &index))
	{
		return m_characters[index];
	}
	else
	{
		m_characters.expand();
		for (int i = m_characters.size() - 1; i > index; i --)
			m_characters[i] = std::move(m_characters[i - 1]);
		m_characters[index].codepoint = codepoint;
		return m_characters[index];
	}
}

const SpriteFont::Character& SpriteFont::get_character(Codepoint codepoint) const
{
	static const Character empty;

	int index;
	if (find_character_index(m_characters, codepoint, 0, m_characters.size() - 1, &index))
		return m_characters[index];

	return empty;
}

SpriteFont::Character& SpriteFont::operator[](Codepoint codepoint)
{
	return get_character(codepoint);
}

const SpriteFont::Character& SpriteFont::operator[](Codepoint codepoint) const
{
	return get_character(codepoint);
}
