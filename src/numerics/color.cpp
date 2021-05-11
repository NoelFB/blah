#include <blah/numerics/color.h>
#include <blah/numerics/vec3.h>
#include <blah/numerics/vec4.h>

using namespace Blah;

char const hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

Vec3 Color::to_vec3() const
{
	return Vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}

Vec4 Color::to_vec4() const
{
	return Vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

String Color::to_hex_rgba() const
{
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
	String str = "000000";
	str[0] = hex[(r & 0xF0) >> 4];
	str[1] = hex[(r & 0x0F) >> 0];
	str[2] = hex[(g & 0xF0) >> 4];
	str[3] = hex[(g & 0x0F) >> 0];
	str[4] = hex[(b & 0xF0) >> 4];
	str[5] = hex[(b & 0x0F) >> 0];
	return str;
}