#pragma once
#include <blah/streams/stream.h>
#include <blah/images/image.h>
#include <blah/containers/str.h>
#include <blah/core/filesystem.h>

namespace Blah
{
	// Loads fonts from file and can blit individual characters to images
	class Font
	{
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

		Font();
		Font(Stream& stream);
		Font(const FilePath& path);
		Font(const Font&) = delete;
		Font& operator=(const Font&) = delete;
		Font(Font&& src) noexcept;
		Font& operator=(Font&& src) noexcept;
		~Font();

		// Releases all Font resources
		// Note that after doing this various properties may become invalid (ex. font name)
		void dispose();

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

		// checks if the Font is valid
		bool is_valid() const;

	private:
		void load(Stream& stream);
		void* m_font;
		unsigned char* m_data;
		String m_family_name;
		String m_style_name;
		int m_ascent;
		int m_descent;
		int m_line_gap;
		bool m_valid;
	};
}