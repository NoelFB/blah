#pragma once
#include <blah_common.h>
#include <blah_string.h>
#include <blah_vector.h>
#include <blah_subtexture.h>
#include <blah_font.h>

namespace Blah
{
	// Sprite Font is a bitmap font implementation for font rendering.
	// It can be constructed from a Font (ttf) and will automatically create 
	// texture atlases for rendering. You can add your own characters
	// and textures to it.
	class SpriteFont
	{
	public:
		
		// Spritefont uses u32 codepoints
		using Codepoint = u32;

		// CharSet is a Vector of Character Ranges
		struct CharRange;
		using CharSet = Vector<CharRange>;

		// Character range, used for building the Sprite Font
		struct CharRange
		{
			Codepoint from;
			Codepoint to;

			CharRange();
			CharRange(Codepoint single);
			CharRange(Codepoint from, Codepoint to);

			static const CharSet ASCII;
		};

		// Character Entry
		struct Character
		{
			Codepoint codepoint = 0;
			int glyph = 0;
			Subtexture subtexture;
			float advance = 0;
			Vec2f offset;
		};

		struct Kerning
		{
			Codepoint a;
			Codepoint b;
			float value;
		};

		// SpriteFont name
		String name;

		// Height, in pixels
		float size = 0;

		// Ascent, in pixels
		float ascent = 0;

		// Descent, in pixels
		float descent = 0;

		// Line Gap, in pixels
		float line_gap = 0;

		SpriteFont() = default;
		SpriteFont(const FilePath& file, float size);
		SpriteFont(const FilePath& file, float size, const CharSet& charset);
		SpriteFont(const FontRef& font, float size);
		SpriteFont(const FontRef& font, float size, const CharSet& charset);

		// releases all assets used by the spritefont
		void clear();

		// gets the height of the sprite font
		float height() const { return ascent - descent; }

		// gets the line height of the sprite font (height + line gap)
		float line_height() const { return ascent - descent + line_gap; }

		// returns a list of all texture atlases
		const Vector<TextureRef>& textures() { return m_atlas; }

		// calculates the width of the given string
		float width_of(const String& text) const;

		// calculates the width of the next line
		float width_of_line(const String& text, int start = 0) const;

		// calculates the height of the given string
		float height_of(const String& text) const;

		// disposes the existing spritefont data and rebuilds from the given font file
		void rebuild(const FilePath& file, float size, const CharSet& charset);

		// disposes the existing spritefont data and rebuilds from the given font
		void rebuild(const FontRef& font, float size, const CharSet& charset);

		// gets the kerning between two characters
		float get_kerning(Codepoint codepoint0, Codepoint codepoint1) const;

		// sets the kerning between two characters
		void set_kerning(Codepoint codepoint0, Codepoint codepoint1, float kerning);

		// gets the character at the given codepoint
		Character& get_character(Codepoint codepoint);

		// gets the character at the given codepoint
		const Character& get_character(Codepoint codepoint) const;

		// gets the character at the given codepoint
		Character& operator[](Codepoint codepoint);

		// gets the character at the given codepoint
		const Character& operator[](Codepoint codepoint) const;

	private:
		Vector<Character> m_characters;
		Vector<Kerning> m_kerning;
		Vector<TextureRef> m_atlas;
	};
}