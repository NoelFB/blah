#pragma once
#include <inttypes.h>
#include <blah/containers/list.h>
#include <blah/containers/str.h>
#include <blah/drawing/subtexture.h>
#include <blah/math/vec2.h>
#include <unordered_map>

namespace Blah
{
	class Font;

	class SpriteFont
	{
	public:
		struct Character
		{
			Subtexture subtexture;
			float advance = 0;
			Vec2 offset;
		};
	private:
		// charset & kerning maps
		std::unordered_map<uint32_t, Character> m_characters;
		std::unordered_map<uint64_t, float> m_kerning;

		// built texture
		List<TextureRef> m_atlas;

	public:
		static const uint32_t* ASCII;

		String name;
		float size;
		float ascent;
		float descent;
		float line_gap;

		// Note:
		// charset is a list of range pairs, until a 0 terminator (ex. 32,128,0)

		SpriteFont();
		SpriteFont(const char* file, float size);
		SpriteFont(const char* file, float size, const uint32_t* charset);
		SpriteFont(const Font& font, float size);
		SpriteFont(const Font& font, float size, const uint32_t* charset);
		SpriteFont(const SpriteFont&) = delete;
		SpriteFont(SpriteFont&& src) noexcept;
		~SpriteFont();

		void dispose();

		SpriteFont& operator=(const SpriteFont&) = delete;
		SpriteFont& operator=(SpriteFont&& src) noexcept;

		float height() const { return ascent - descent; }
		float line_height() const { return ascent - descent + line_gap; }

		List<TextureRef>& textures() { return m_atlas; }

		float width_of(const String& text) const;
		float width_of_line(const String& text, int start = 0) const;
		float height_of(const String& text) const;

		void build(const char* file, float size, const uint32_t* charset);
		void build(const Font& font, float size, const uint32_t* charset);

		float get_kerning(uint32_t codepoint0, uint32_t codepoint1) const;
		void set_kerning(uint32_t codepoint0, uint32_t codepoint1, float kerning);

		Character& get_character(uint32_t codepoint) { return m_characters[codepoint]; }
		const Character& get_character(uint32_t codepoint) const;
		Character& operator[](uint32_t codepoint) { return m_characters[codepoint]; }
		const Character& operator[](uint32_t codepoint) const;
	};
}