#pragma once
#include <blah/core/common.h>
#include <blah/containers/str.h>
#include <blah/containers/vector.h>
#include <blah/drawing/subtexture.h>
#include <blah/math/vec2.h>
#include <blah/core/filesystem.h>
#include <unordered_map>

namespace Blah
{
	class Font;

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
			Subtexture subtexture;
			float advance = 0;
			Vec2 offset;
		};

		// SpriteFont name
		String name;

		// Height, in pixels
		float size;

		// Ascent, in pixels
		float ascent;

		// Descent, in pixels
		float descent;

		// Line Gap, in pixels
		float line_gap;

		SpriteFont();
		SpriteFont(const FilePath& file, float size);
		SpriteFont(const FilePath& file, float size, const CharSet& charset);
		SpriteFont(const Font& font, float size);
		SpriteFont(const Font& font, float size, const CharSet& charset);
		SpriteFont(const SpriteFont&) = delete;
		SpriteFont& operator=(const SpriteFont&) = delete;
		SpriteFont(SpriteFont&& src) noexcept;
		SpriteFont& operator=(SpriteFont&& src) noexcept;
		~SpriteFont();

		// releases all assets used by the spritefont
		void dispose();

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
		void rebuild(const Font& font, float size, const CharSet& charset);

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
		// character set
		std::unordered_map<Codepoint, Character> m_characters;

		// kerning
		// key is 2 codepoints combined ((first << 32) | second)
		std::unordered_map<u64, float> m_kerning;

		// built texture
		Vector<TextureRef> m_atlas;
	};
}