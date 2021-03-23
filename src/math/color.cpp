#include <blah/math/color.h>
#include <blah/math/vec3.h>
#include <blah/math/vec4.h>

using namespace Blah;

char const hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
#define LT_HEX_VALUE(n) ((n >= '0' && n <= '9') ? (n - '0') : ((n >= 'A' && n <= 'F') ? (10 + n - 'A') : ((n >= 'a' && n <= 'f') ? (10 + n - 'a') : 0)))

Color::Color()
	: r(0)
	, g(0)
	, b(0)
	, a(0) {}

Color::Color(int rgb)
	: r((u8)((rgb & 0xFF0000) >> 16))
	, g((u8)((rgb & 0x00FF00) >> 8))
	, b((u8)(rgb & 0x0000FF))
	, a(255) {}

Color::Color(int rgb, float alpha)
	: r((int)(((u8)((rgb & 0xFF0000) >> 16)) * alpha))
	, g((int)(((u8)((rgb & 0x00FF00) >> 8)) * alpha))
	, b((int)(((u8)(rgb & 0x0000FF)) * alpha))
	, a((int)(255 * alpha)) {}

Color::Color(u8 r, u8 g, u8 b)
	: r(r)
	, g(g)
	, b(b)
	, a(255) {}

Color::Color(u8 r, u8 g, u8 b, u8 a)
	: r(r)
	, g(g)
	, b(b)
	, a(a) {}

Color::Color(const Vec3& vec3)
	: r((int)(vec3.x * 255))
	, g((int)(vec3.y * 255))
	, b((int)(vec3.z * 255))
	, a((int)(255)) {}

Color::Color(const Vec3& vec3, float alpha)
	: r((int)(vec3.x * alpha * 255))
	, g((int)(vec3.y* alpha * 255))
	, b((int)(vec3.z* alpha * 255))
	, a((int)(alpha * 255)) {}

Color::Color(const Vec4& vec4)
	: r((int)(vec4.x * 255))
	, g((int)(vec4.y * 255))
	, b((int)(vec4.z * 255))
	, a((int)(vec4.w * 255)) {}

Color::Color(const String& value) 
	: r(0)
	, g(0)
	, b(0)
	, a(255)
{
	int offset = 0;
	if (value.length() > 0 && value[0] == '#')
		offset = 1;
	else if (value.length() >= 1 && value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
		offset = 2;

	if (value.length() - offset >= 8)
		a = (LT_HEX_VALUE(value[offset + 6]) << 4) + LT_HEX_VALUE(value[offset + 7]);

	if (value.length() - offset >= 6)
	{
		r = (LT_HEX_VALUE(value[offset + 0]) << 4) + LT_HEX_VALUE(value[offset + 1]);
		g = (LT_HEX_VALUE(value[offset + 2]) << 4) + LT_HEX_VALUE(value[offset + 3]);
		b = (LT_HEX_VALUE(value[offset + 4]) << 4) + LT_HEX_VALUE(value[offset + 5]);
	}
}

void Color::premultiply()
{
	r = r * a / 255;
	g = g * a / 255;
	b = b * a / 255;
}

u32 Color::to_rgba() const
{
	return
		((u32)r << 24) |
		((u32)g << 16) |
		((u32)b << 8) |
		(u32)a;
}

Vec4 Color::to_vec4() const
{
	return Vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

void Color::to_hex_rgba(char* buffer) const
{
	buffer[0] = hex[(r & 0xF0) >> 4];
	buffer[1] = hex[(r & 0x0F) >> 0];
	buffer[2] = hex[(g & 0xF0) >> 4];
	buffer[3] = hex[(g & 0x0F) >> 0];
	buffer[4] = hex[(b & 0xF0) >> 4];
	buffer[5] = hex[(b & 0x0F) >> 0];
	buffer[6] = hex[(a & 0xF0) >> 4];
	buffer[7] = hex[(a & 0x0F) >> 0];
}

String Color::to_hex_rgba() const
{
	String str = "00000000";
	to_hex_rgba(str.cstr());
	return str;
}

void Color::to_hex_rgb(char* buffer) const
{
	buffer[0] = hex[(r & 0xF0) >> 4];
	buffer[1] = hex[(r & 0x0F) >> 0];
	buffer[2] = hex[(g & 0xF0) >> 4];
	buffer[3] = hex[(g & 0x0F) >> 0];
	buffer[4] = hex[(b & 0xF0) >> 4];
	buffer[5] = hex[(b & 0x0F) >> 0];
}

String Color::to_hex_rgb() const
{
	String str = "000000";
	to_hex_rgb(str.cstr());
	return str;
}

Color Color::from_rgba(u32 value)
{
	return
	{
		(u8)((value & 0xFF000000) >> 24),
		(u8)((value & 0x00FF0000) >> 16),
		(u8)((value & 0x0000FF00) >> 8),
		(u8)((value & 0x000000FF))
	};
}

Color Color::from_rgb(u32 value)
{
	return
	{
		(u8)((value & 0xFF0000) >> 16),
		(u8)((value & 0x00FF00) >> 8),
		(u8)((value & 0x0000FF))
	};
}

Color Color::lerp(Color a, Color b, float amount)
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

Color Color::operator*(float multiply) const
{
	return Color(
		(int)(r * multiply),
		(int)(g * multiply),
		(int)(b * multiply),
		(int)(a * multiply));
}

Color& Color::operator=(const int rgb)
{
	r = (u8)((rgb & 0xFF0000) >> 16);
	g = (u8)((rgb & 0x00FF00) >> 8);
	b = (u8)(rgb & 0x0000FF);
	a = 255;
	return *this;
}

bool Color::operator ==(const Color& rhs) const { return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a; }
bool Color::operator !=(const Color& rhs) const { return r != rhs.r || g != rhs.g || b != rhs.b || a != rhs.a; }

const Color Color::transparent = Color(0, 0, 0, 0);
const Color Color::white = Color(255, 255, 255, 255);
const Color Color::black = Color(0, 0, 0, 255);
const Color Color::red = Color(255, 0, 0, 255);
const Color Color::green = Color(0, 255, 0, 255);
const Color Color::blue = Color(0, 0, 255, 255);
const Color Color::yellow = Color(255, 255, 0, 255);
const Color Color::purple = Color(255, 0, 255, 255);
const Color Color::teal = Color(0, 255, 255, 255);