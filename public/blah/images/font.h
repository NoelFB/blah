#pragma once
#include <blah/streams/stream.h>
#include <blah/images/image.h>
#include <blah/containers/str.h>

namespace Blah
{
	typedef uint32_t Codepoint;

	class Font
	{
	public:
		struct Char
		{
			int glyph = 0;
			int width = 0;
			int height = 0;
			float advance = 0;
			float offsetX = 0;
			float offsetY = 0;
			float scale = 0;
			bool hasGlyph = false;
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

		const char* FamilyName() const;
		const char* StyleName() const;
		int Ascent() const;
		int Descent() const;
		int LineGap() const;
		int Height() const;
		int LineHeight() const;

		int GetGlyph(Codepoint codepoint) const;
		float GetScale(float size) const;
		float GetKerning(int glyph1, int glyph2, float scale) const;
		Char GetCharacter(int glyph, float scale) const;
		bool GetBitmap(const Char& ch, Color* pixels) const;
		bool IsValid() const;

	private:
		void Load(Stream& stream);
		void* font;
		unsigned char* data;
		String familyName;
		String styleName;
		int ascent;
		int descent;
		int lineGap;
		bool valid;
	};
}