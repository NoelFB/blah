#include <blah_string.h>

using namespace Blah;

namespace
{
	constexpr char blah_to_lower(char c)
	{
		if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
		return c;
	}

	constexpr int blah_strlen(const char* cstr)
	{
		int len = 0;
		if (cstr)
			while (*(cstr + len) != '\0' && len < INT32_MAX) len++;
		return len;
	}

	constexpr bool blah_compare_ignore_case(char a, char b) { return blah_to_lower(a) == blah_to_lower(b); };
	constexpr bool blah_compare_with_case(char a, char b) { return a == b; };
}

void BaseString::assign(const char* cstr, const char* cstr_end)
{
	s_clear();
	append(cstr, cstr_end);
}

void BaseString::append(const char* cstr, const char* cstr_end)
{
	// make sure values are valid
	if (cstr == nullptr || *cstr == '\0')
		return;
	if (cstr_end == nullptr)
		cstr_end = cstr + blah_strlen(cstr);

	// reserve (+1 for null-terminator)
	auto len = length();
	s_ensure(len + (cstr_end - cstr) + 1);

	// copy value over to our buffer
	char* dst = s_ptr() + len;
	while (cstr < cstr_end)
		*(dst++) = *(cstr++);
}

void BaseString::append(const u16* u16_cstr, const u16* u16_cstr_end, bool swap_endian)
{
	// converts utf16 into utf8
	// more info: https://en.wikipedia.org/wiki/UTF-16#Description

	const u16 surrogate_min = 0xd800u;
	const u16 surrogate_max = 0xdbffu;

	while ((u16_cstr_end == nullptr && *u16_cstr != 0) || (u16_cstr_end != nullptr && u16_cstr != u16_cstr_end))
	{
		u16 next = (*u16_cstr++);
		if (swap_endian)
			next = ((next & 0xff) << 8 | ((next & 0xff00) >> 8));

		u32 cp = 0xffff & next;

		if ((cp >= surrogate_min && cp <= surrogate_max))
		{
			next = (*u16_cstr++);
			if (swap_endian)
				next = ((next & 0xff) << 8 | ((next & 0xff00) >> 8));

			u32 trail = 0xffff & next;
			cp = (cp << 10) + trail + 0x10000u - (surrogate_min << 10) - 0xdc00u;
		}

		append(cp);
	}
}

void BaseString::append(char ch, int count)
{
	// reserve (+1 for null-terminator)
	auto len = length();
	s_ensure(len + count + 1);

	// copy value over to our buffer
	char* dst = s_ptr() + len;
	char* end = dst + count;
	while (dst < end)
		*(dst++) = ch;
}

void BaseString::append(u32 c)
{
	// one octet
	if (c < 0x80)
	{
		append((char)c);
	}
	// two octets
	else if (c < 0x800)
	{
		append((char)((c >> 6) | 0xc0));
		append((char)((c & 0x3f) | 0x80));
	}
	// three octets
	else if (c < 0x10000)
	{
		append((char)((c >> 12) | 0xe0));
		append((char)(((c >> 6) & 0x3f) | 0x80));
		append((char)((c & 0x3f) | 0x80));
	}
	// four octets
	else
	{
		append((char)((c >> 18) | 0xf0));
		append((char)(((c >> 12) & 0x3f) | 0x80));
		append((char)(((c >> 6) & 0x3f) | 0x80));
		append((char)((c & 0x3f) | 0x80));
	}
}

void BaseString::append_fmt(const char* fmt, ...)
{
	va_list args;

	// determine arg m_length
	va_start(args, fmt);
	auto add = vsnprintf(nullptr, 0, fmt, args);
	va_end(args);

	if (add <= 0)
		return;

	// reserve (+1 for null-terminator)
	auto len = length();
	s_ensure(len + add + 1);

	// print out
	va_start(args, fmt);
	vsnprintf(s_ptr() + len, add + 1, fmt, args);
	va_end(args);
}

bool BaseString::starts_with(const char* cstr, bool ignore_case) const
{
	if (cstr == nullptr || *cstr == '\0')
		return length() == 0;

	const char* ptr = s_ptr();
	auto compare = (ignore_case ? &blah_compare_ignore_case : &blah_compare_with_case);

	while (*cstr != '\0')
	{
		if (!(*compare)(*cstr, *ptr))
			return false;
		cstr++; ptr++;
	}

	return true;
}

bool BaseString::ends_with(const char* cstr, bool ignore_case) const
{
	if (cstr == nullptr || *cstr == '\0')
		return length() == 0;

	int len = blah_strlen(cstr);
	if (len > length())
		return false;

	const char* ptr = s_ptr() + length() - len;
	auto compare = (ignore_case ? &blah_compare_ignore_case : &blah_compare_with_case);

	while (*cstr != '\0')
	{
		if (!(*compare)(*cstr, *ptr))
			return false;
		cstr++; ptr++;
	}

	return true;
}

bool BaseString::contains(const char* cstr, bool ignore_case) const
{
	if (cstr == nullptr || *cstr == '\0')
		return length() == 0;

	int len = blah_strlen(cstr);
	if (len > length())
		return false;

	const char* ptr = s_ptr();
	const char* end = s_ptr() + length() - len + 1;
	auto compare = (ignore_case ? &blah_compare_ignore_case : &blah_compare_with_case);

	while (ptr < end)
	{
		const char* a = ptr;
		const char* b = cstr;

		while (*b != '\0')
		{
			if (!(*compare)(*a, *b))
				goto NEXT;
			a++; b++;
		}

		return true;
	NEXT:
		ptr++;
	}

	return false;
}

int BaseString::first_index_of(char ch) const
{
	const char* ptr = s_ptr();
	const char* end = ptr + length();

	while (ptr < end)
	{
		if (*ptr == ch)
			return ptr - s_ptr();
		ptr++;
	}

	return -1;
}

int BaseString::last_index_of(char ch) const
{
	const char* ptr = s_ptr();
	const char* end = ptr + length() - 1;

	while (end >= ptr)
	{
		if (*end == ch)
			return end - s_ptr();
		end--;
	}

	return -1;
}

bool BaseString::equals(const char* other, bool ignore_case) const
{
	const char* a = s_ptr(); const char* b = other;
	auto compare = (ignore_case ? &blah_compare_ignore_case : &blah_compare_with_case);
	while ((*compare)(*a, *b) && *a != '\0') { a++; b++; }
	return (*compare)(*a, *b);
}

namespace
{
	u32 utf8_character_at(const char* str)
	{
		u32 charcode = 0;

		int t = (unsigned char)(*(str++));
		if (t < 128)
			return t;

		int high_bit_mask = (1 << 6) - 1;
		int high_bit_shift = 0;
		int total_bits = 0;
		int other_bits = 6;

		while ((t & 0xC0) == 0xC0)
		{
			t <<= 1;
			t &= 0xff;
			total_bits += 6;
			high_bit_mask >>= 1;
			high_bit_shift++;
			charcode <<= other_bits;
			charcode |= ((unsigned char)(*(str++))) & ((1 << other_bits) - 1);
		}
		charcode |= ((t >> high_bit_shift) & high_bit_mask) << total_bits;

		return charcode;
	}

	u32 utf8_character_size(const char* str)
	{
		char c = *str;
		if (c == '\0')
			return 0;
		if ((c & 0xFE) == 0xFC)
			return 6;
		if ((c & 0xFC) == 0xF8)
			return 5;
		if ((c & 0xF8) == 0xF0)
			return 4;
		if ((c & 0xF0) == 0xE0)
			return 3;
		if ((c & 0xE0) == 0xC0)
			return 2;
		return 1;
	}
}

Utf8::Utf8(const char* cstr)
	: str(cstr)
{
	character = utf8_character_at(str);
	character_size = utf8_character_size(str);
}

bool Utf8::next()
{
	str += character_size;
	character = utf8_character_at(str);
	character_size = utf8_character_size(str);
	return character_size > 0;
}