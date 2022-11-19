#pragma once
#include <blah_common.h>
#include <blah_string.h>
#include <blah_spatial.h>

#define BLAH_HEX_VALUE(n) ((n >= '0' && n <= '9') ? (n - '0') : ((n >= 'A' && n <= 'F') ? (10 + n - 'A') : ((n >= 'a' && n <= 'f') ? (10 + n - 'a') : 0)))

namespace Blah
{
	struct Color
	{
		u8 r;
		u8 g;
		u8 b;
		u8 a;

		constexpr Color()
			: r(0), g(0), b(0), a(0) {}

		constexpr Color(int rgb)
			: r((u8)((rgb & 0xFF0000) >> 16))
			, g((u8)((rgb & 0x00FF00) >> 8))
			, b((u8)(rgb & 0x0000FF))
			, a(255) {}

		constexpr Color(int rgb, float alpha)
			: r((int)(((u8)((rgb & 0xFF0000) >> 16)) * alpha))
			, g((int)(((u8)((rgb & 0x00FF00) >> 8)) * alpha))
			, b((int)(((u8)(rgb & 0x0000FF)) * alpha))
			, a((int)(255 * alpha)) {}

		constexpr Color(u8 r, u8 g, u8 b)
			: r(r)
			, g(g)
			, b(b)
			, a(255) {}

		constexpr Color(u8 r, u8 g, u8 b, u8 a)
			: r(r)
			, g(g)
			, b(b)
			, a(a) {}

		constexpr Color(const Vec3f& vec3)
			: r((int)(vec3.x * 255))
			, g((int)(vec3.y * 255))
			, b((int)(vec3.z * 255))
			, a((int)(255)) {}

		constexpr Color(const Vec3f& vec3, float alpha)
			: r((int)(vec3.x* alpha * 255))
			, g((int)(vec3.y* alpha * 255))
			, b((int)(vec3.z* alpha * 255))
			, a((int)(alpha * 255)) {}

		constexpr Color(const Vec4f& vec4)
			: r((int)(vec4.x * 255))
			, g((int)(vec4.y * 255))
			, b((int)(vec4.z * 255))
			, a((int)(vec4.w * 255)) {}

		// Parses a Hex string in the format of "#00000000" or "0x00000000" or "00000000"
		constexpr Color(const char* hex_string)
			: r(0), g(0), b(0), a(255)
		{
			if (*hex_string == '#')
				hex_string += 1;
			else if (*hex_string == '0' && (*(hex_string + 1) == 'x' || *(hex_string + 1) == 'X'))
				hex_string += 2;

			int len = 0;
			while (len < 8 && *(hex_string + len) != '\0')
				len++;

			if (len >= 6)
			{
				r = (BLAH_HEX_VALUE(hex_string[0]) << 4) + BLAH_HEX_VALUE(hex_string[1]);
				g = (BLAH_HEX_VALUE(hex_string[2]) << 4) + BLAH_HEX_VALUE(hex_string[3]);
				b = (BLAH_HEX_VALUE(hex_string[4]) << 4) + BLAH_HEX_VALUE(hex_string[5]);
			}

			if (len >= 8)
				a = (BLAH_HEX_VALUE(hex_string[6]) << 4) + BLAH_HEX_VALUE(hex_string[7]);
		}

		// Premultiplies the Color
		constexpr void premultiply()
		{
			r = r * a / 255;
			g = g * a / 255;
			b = b * a / 255;
		}

		// Returns an RGBA hex string of the color
		String to_hex_rgba() const
		{
			static const char* hex = "0123456789ABCDEF";

			String str = "00000000";
			str[0] = hex[(r & 0xF0) >> 4];
			str[1] = hex[(r & 0x0F) >> 0];
			str[2] = hex[(g & 0xF0) >> 4];
			str[3] = hex[(g & 0x0F) >> 0];
			str[4] = hex[(b & 0xF0) >> 4];
			str[5] = hex[(b & 0x0F) >> 0];
			str[6] = hex[(a & 0xF0) >> 4];
			str[7] = hex[(a & 0x0F) >> 0];
			return str;
		}

		// Returns an RGB hex string of the color
		String to_hex_rgb() const
		{
			static const char* hex = "0123456789ABCDEF";

			String str = "000000";
			str[0] = hex[(r & 0xF0) >> 4];
			str[1] = hex[(r & 0x0F) >> 0];
			str[2] = hex[(g & 0xF0) >> 4];
			str[3] = hex[(g & 0x0F) >> 0];
			str[4] = hex[(b & 0xF0) >> 4];
			str[5] = hex[(b & 0x0F) >> 0];
			return str;
		}

		// Converts the Color to a Vec3 (RGB)
		constexpr Vec3f to_vec3() const
		{
			return Vec3f(r / 255.0f, g / 255.0f, b / 255.0f);
		}

		// Converts the Color to a Vec4 (RGBA)
		constexpr Vec4f to_vec4() const
		{
			return Vec4f(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
		}

		// Converts the Color to a u32
		constexpr u32 to_rgba() const
		{
			return
				((u32)r << 24) |
				((u32)g << 16) |
				((u32)b << 8) |
				(u32)a;
		}

		// Converts the Color to a u32
		constexpr u32 to_rgb() const
		{
			return
				((u32)r << 16) |
				((u32)g << 8) |
				(u32)b;
		}

		// Returns an RGBA Color representation of the integer value
		static constexpr Color from_rgba(u32 value)
		{
			return
			{
				(u8)((value & 0xFF000000) >> 24),
				(u8)((value & 0x00FF0000) >> 16),
				(u8)((value & 0x0000FF00) >> 8),
				(u8)((value & 0x000000FF))
			};
		}

		// Returns an RGB Color representation of the integer value, with Alpha = 255
		static constexpr Color from_rgb(u32 value)
		{
			return
			{
				(u8)((value & 0xFF0000) >> 16),
				(u8)((value & 0x00FF00) >> 8),
				(u8)((value & 0x0000FF))
			};
		}
		
		// Lerps between two colors
		static constexpr Color lerp(Color a, Color b, float amount)
		{
			if (amount < 0) amount = 0;
			if (amount > 1) amount = 1;

			return Color(
				(u8)(a.r + (b.r - a.r) * amount),
				(u8)(a.g + (b.g - a.g) * amount),
				(u8)(a.b + (b.b - a.b) * amount),
				(u8)(a.a + (b.a - a.a) * amount)
			);
		}

		// Multiples the Color
		constexpr Color operator*(float multiply) const
		{
			return Color(
				(int)(r * multiply),
				(int)(g * multiply),
				(int)(b * multiply),
				(int)(a * multiply));
		}

		// assignment from int
		constexpr Color& operator= (int rgb)
		{
			r = (u8)((rgb & 0xFF0000) >> 16);
			g = (u8)((rgb & 0x00FF00) >> 8);
			b = (u8)(rgb & 0x0000FF);
			a = 255;
			return *this;
		}

		constexpr bool operator ==(const Color& rhs) const
		{
			return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
		}

		constexpr bool operator !=(const Color& rhs) const
		{
			return r != rhs.r || g != rhs.g || b != rhs.b || a != rhs.a;
		}
		
		static const Color transparent;
		static const Color white;
		static const Color black;
		static const Color red;
		static const Color green;
		static const Color blue;
		static const Color yellow;
		static const Color orange;
		static const Color purple;
		static const Color teal;
	};

	inline const Color Color::transparent = Color(  0,   0,   0,   0);
	inline const Color Color::white       = Color(255, 255, 255);
	inline const Color Color::black       = Color(  0,   0,   0);
	inline const Color Color::red         = Color(255,   0,   0);
	inline const Color Color::green       = Color(  0, 255,   0);
	inline const Color Color::blue        = Color(  0,   0, 255);
	inline const Color Color::yellow      = Color(255, 255,   0);
	inline const Color Color::orange      = Color(255, 165,   0);
	inline const Color Color::purple      = Color(255,   0, 255);
	inline const Color Color::teal        = Color(  0, 255, 255);
}

#undef BLAH_HEX_VALUE
