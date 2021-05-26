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