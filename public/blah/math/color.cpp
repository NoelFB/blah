#include <blah/math/color.h>
#include <blah/math/vec4.h>

using namespace Blah;

char const hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
#define LT_HEX_VALUE(n) ((n >= '0' && n <= '9') ? (n - '0') : ((n >= 'A' && n <= 'F') ? (10 + n - 'A') : ((n >= 'a' && n <= 'f') ? (10 + n - 'a') : 0)))

Color::Color()
	: r(0), g(0), b(0), a(0) {}

Color::Color(int rgb) :
	r((uint8_t)((rgb & 0xFF0000) >> 16)),
	g((uint8_t)((rgb & 0x00FF00) >> 8)),
	b((uint8_t)(rgb & 0x0000FF)),
	a(255) {}

Color::Color(int rgb, float alpha) :
	r((int)(((uint8_t)((rgb & 0xFF0000) >> 16)) * alpha)),
	g((int)(((uint8_t)((rgb & 0x00FF00) >> 8)) * alpha)),
	b((int)(((uint8_t)(rgb & 0x0000FF)) * alpha)),
	a((int)(255 * alpha)) {}

Color::Color(uint8_t r, uint8_t g, uint8_t b)
	: r(r), g(g), b(b), a(255) {}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	: r(r), g(g), b(b), a(a) {}

Color::Color(const char* value) : r(0), g(0), b(0), a(255)
{
	int length = 0;
	while (value[length] != '\0' && length < 10)
		length ++;

	int offset = 0;
	if (length > 0 && value[0] == '#')
		offset = 1;
	else if (length >= 1 && value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
		offset = 2;

	if (length - offset >= 8)
		a = (LT_HEX_VALUE(value[offset + 6]) << 4) + LT_HEX_VALUE(value[offset + 7]);

	if (length - offset >= 6)
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

uint32_t Color::to_rgba() const
{
	return
		((uint32_t)r << 24) |
		((uint32_t)g << 16) |
		((uint32_t)b << 8) |
		(uint32_t)a;
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

void Color::to_hex_rgb(char* buffer) const
{
	buffer[0] = hex[(r & 0xF0) >> 4];
	buffer[1] = hex[(r & 0x0F) >> 0];
	buffer[2] = hex[(g & 0xF0) >> 4];
	buffer[3] = hex[(g & 0x0F) >> 0];
	buffer[4] = hex[(b & 0xF0) >> 4];
	buffer[5] = hex[(b & 0x0F) >> 0];
}

Color Color::from_rgba(uint32_t value)
{
	return
	{
		(uint8_t)((value & 0xFF000000) >> 24),
		(uint8_t)((value & 0x00FF0000) >> 16),
		(uint8_t)((value & 0x0000FF00) >> 8),
		(uint8_t)((value & 0x000000FF))
	};
}

Color Color::from_rgb(uint32_t value)
{
	return
	{
		(uint8_t)((value & 0xFF0000) >> 16),
		(uint8_t)((value & 0x00FF00) >> 8),
		(uint8_t)((value & 0x0000FF))
	};
}

Color Color::lerp(Color a, Color b, float amount)
{
	if (amount < 0) amount = 0;
	if (amount > 1) amount = 1;

	return Color(
		(uint8_t)(a.r + (b.r - a.r) * amount),
		(uint8_t)(a.g + (b.g - a.g) * amount),
		(uint8_t)(a.b + (b.b - a.b) * amount),
		(uint8_t)(a.a + (b.a - a.a) * amount)
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

Color& Color::operator= (const int rgb)
{
	r = (uint8_t)((rgb & 0xFF0000) >> 16);
	g = (uint8_t)((rgb & 0x00FF00) >> 8);
	b = (uint8_t)(rgb & 0x0000FF);
	a = 255;
	return *this;
}

const Color Color::transparent = Color(0, 0, 0, 0);
const Color Color::white = Color(255, 255, 255, 255);
const Color Color::black = Color(0, 0, 0, 255);
const Color Color::red = Color(255, 0, 0, 255);
const Color Color::green = Color(0, 255, 0, 255);
const Color Color::blue = Color(0, 0, 255, 255);
const Color Color::yellow = Color(255, 255, 0, 255);
const Color Color::purple = Color(255, 0, 255, 255);
const Color Color::teal = Color(0, 255, 255, 255);