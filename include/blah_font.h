#pragma once
#include <blah_common.h>
#include <blah_stream.h>
#include <blah_image.h>
#include <blah_string.h>
#include <blah_vector.h>
#include <blah_filesystem.h>

namespace Blah
{
	class Font;
	using FontRef = Ref<Font>;

	// Loads fonts from file and can blit individual characters to images.
	// Use Font::create() to instantiate a FontRef.
	class Font
	{
	private:
		Font() = default;

	public:

		// Font uses u32 Codepoints
		using Codepoint = u32;

		// Information provided for a single Character
		struct Character
		{
			// character's glyph index
			int glyph = 0;

			// width of the character, in pixels
			int width = 0;

			// height of the character, in pixels
			int height = 0;

			// advance (how much to move horizontally)
			float advance = 0;

			// render x-offset
			float offset_x = 0;

			// render y-offset
			float offset_y = 0;

			// scale the character was created at
			float scale = 0;

			// whether the character has a visible glyph
			bool has_glyph = false;
		};

		// creates a new font from the given file path
		static FontRef create(const FilePath& path);

		// creates a new font from the Stream
		static FontRef create(Stream& stream);

		// returns the font family name
		const String& family_name() const;

		// returns the font style name
		const String& style_name() const;

		// the font ascent
		int ascent() const;

		// the font descent
		int descent() const;

		// the font line gap (space between lines)
		int line_gap() const;

		// the height of the font
		int height() const;

		// the height of the line, including line gap
		int line_height() const;

		// gets the glyph index for the given codepoint
		int get_glyph(Codepoint codepoint) const;

		// gets the font scale for the given font size in pixels
		float get_scale(float size) const;

		// gets the font kerning between 2 glyphs
		float get_kerning(int glyph1, int glyph2, float scale) const;

		// gets character data for the given glyph, at the provided scale
		Character get_character(int glyph, float scale) const;

		// Blits a character to the provided pixel array.
		// The pixel array must be at least ch.width * ch.height in size!
		// If the character doesn't exist, this will do nothing and return false.
		bool get_image(const Character& ch, Color* pixels) const;

		// Returns an image of the provided character.
		// If the character doesn't exist, this will return an empty image.
		Image get_image(const Character& ch) const;

	private:
		Ref<void> m_font;
		Vector<u8> m_buffer;
		String m_family_name;
		String m_style_name;
		int m_ascent = 0;
		int m_descent = 0;
		int m_line_gap = 0;
	};
}