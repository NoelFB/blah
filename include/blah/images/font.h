#pragma once
#include <blah/streams/stream.h>
#include <blah/images/image.h>
#include <blah/containers/str.h>

namespace Blah
{
	typedef u32 Codepoint;

	class Font
	{
	public:
		struct Char
		{
			int glyph = 0;
			int width = 0;
			int height = 0;
			float advance = 0;
			float offset_x = 0;
			float offset_y = 0;
			float scale = 0;
			bool has_glyph = false;
		};

		Font();
		Font(Stream& stream);
		Font(const char* path);
		Font(const Font&) = delete;
		Font& operator=(const Font&) = delete;
		Font(Font&& src) noexcept;
		Font& operator=(Font&& src) noexcept;
		~Font();

		void dispose();

		const char* family_name() const;
		const char* style_name() const;
		int ascent() const;
		int descent() const;
		int line_gap() const;
		int height() const;
		int line_height() const;

		int get_glyph(Codepoint codepoint) const;
		float get_scale(float size) const;
		float get_kerning(int glyph1, int glyph2, float scale) const;
		Char get_character(int glyph, float scale) const;
		bool get_image(const Char& ch, Color* pixels) const;
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