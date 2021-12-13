#include <blah/numerics/color.h>

using namespace Blah;

String Color::to_hex_rgba() const
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

String Color::to_hex_rgb() const
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

const Color Color::transparent = Color(0, 0, 0, 0);
const Color Color::white = Color(255, 255, 255, 255);
const Color Color::black = Color(0, 0, 0, 255);
const Color Color::red = Color(255, 0, 0, 255);
const Color Color::green = Color(0, 255, 0, 255);
const Color Color::blue = Color(0, 0, 255, 255);
const Color Color::yellow = Color(255, 255, 0, 255);
const Color Color::purple = Color(255, 0, 255, 255);
const Color Color::teal = Color(0, 255, 255, 255);