#include <blah/containers/str.h>
#include <string.h> // for strcpy etc
#include <stdarg.h> // for format methods
#include <stdio.h>	// for sprintf
#include <cctype>	// toupper

using namespace Blah;

const uint16_t UTF8_LEAD_SURROGATE_MIN = 0xd800u;
const uint16_t UTF8_LEAD_SURROGATE_MAX = 0xdbffu;
const uint16_t UTF8_TRAIL_SURROGATE_MIN = 0xdc00u;
const uint16_t UTF8_TRAIL_SURROGATE_MAX = 0xdfffu;
const uint16_t UTF8_LEAD_OFFSET = UTF8_LEAD_SURROGATE_MIN - (0x10000 >> 10);
const uint32_t UTF8_SURROGATE_OFFSET = 0x10000u - (UTF8_LEAD_SURROGATE_MIN << 10) - UTF8_TRAIL_SURROGATE_MIN;

char Str::empty_buffer[1] = { '\0' };

bool Str::operator==(const Str& rhs) const
{
	return strcmp(cstr(), rhs.cstr()) == 0;
}

bool Str::operator!=(const Str& rhs) const
{
	return strcmp(cstr(), rhs.cstr()) != 0;
}

bool Str::operator==(const char* rhs) const
{
	return strcmp(cstr(), rhs) == 0;
}

bool Str::operator!=(const char* rhs) const
{
	return strcmp(cstr(), rhs) != 0;
}

void Str::reserve(int size)
{
	int buffer_length = size + 1;
	if (buffer_length > m_capacity)
	{
		if (m_capacity <= 0)
			m_capacity = 16;

		while (m_capacity < buffer_length)
			m_capacity *= 2;

		// expand from local buffer
		if (m_buffer == nullptr)
		{
			char* local = data();
			m_buffer = new char[m_capacity];
			strncpy(m_buffer, local, m_local_size);
			m_buffer[m_local_size] = '\0';
		}
		// expand from empty buffer
		else if (m_buffer == empty_buffer)
		{
			m_buffer = new char[m_capacity];
			m_buffer[0] = '\0';
		}
		// expand from existing heap buffer
		else
		{
			char* new_buffer = new char[m_capacity];
			memcpy(new_buffer, m_buffer, m_length);
			new_buffer[m_length] = '\0';
			delete[] m_buffer;
			m_buffer = new_buffer;
		}
	}
}

void Str::set_length(int len)
{
	reserve(len);
	data()[len] = '\0';
	m_length = len;
}

uint32_t Str::utf8_at(int index) const
{
	uint32_t charcode = 0;
	
	int t = (unsigned char)(this->operator[](index++));
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
		charcode |= ((unsigned char)(this->operator[](index++))) & ((1 << other_bits) - 1);
	}
	charcode |= ((t >> high_bit_shift) & high_bit_mask) << total_bits;

	return charcode;
}

int Str::utf8_length(int index) const
{
	auto c = this->operator[](index);
	if ((c & 0xFE) == 0xFC)
		return 6;
	if ((c & 0xFC) == 0xF8)
		return 5;
	if ((c & 0xF8) == 0xF0)
		return 4;
	else if ((c & 0xF0) == 0xE0)
		return 3;
	else if ((c & 0xE0) == 0xC0)
		return 2;
	return 1;
}

Str& Str::append(char c)
{
	reserve(m_length + 1);
	data()[m_length++] = c;
	data()[m_length] = '\0';
	return *this;
}

Str& Str::append(uint32_t c)
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

	return *this;
}

Str& Str::append(const char* start, const char* end)
{
	if (end == nullptr)
		end = start + strlen(start);
	
	int add = (int)(end - start);
	if (add <= 0)
		return *this;

	reserve(m_length + add);
	memcpy(data() + m_length, start, add);
	m_length += add;
	data()[m_length] = '\0';

	return *this;
}

Str& Str::append(const Str& str, int start, int end)
{
	if (end < 0)				end = str.m_length;
	if (end > str.m_length)		end = str.m_length;
	if (start < 0)				start = 0;
	if (start > end)			start = end;

	return append(str.begin() + start, str.begin() + end);
}

Str& Str::append_fmt(const char* fmt, ...)
{
	int add, diff;

	// determine arg m_length
	va_list args;
	va_start(args, fmt);
	add = vsnprintf(NULL, 0, fmt, args);
	va_end(args);

	// reserve
	reserve(m_length + add);
	diff = m_capacity - m_length;
	if (diff <= 0) diff = 0;

	// print out
	va_start(args, fmt);
	vsnprintf(data() + m_length, (size_t)diff, fmt, args);
	va_end(args);

	// increment size
	m_length += add;
	data()[m_length] = '\0';

	return *this;
}

Str& Str::append_utf16(const uint16_t* start, const uint16_t* end, bool swapEndian)
{
	while (start != end)
	{
		uint16_t next = (*start++);
		if (swapEndian)
			next = ((next & 0xff) << 8 | ((next & 0xff00) >> 8));

		uint32_t cp = 0xffff & next;

		if ((cp >= UTF8_LEAD_SURROGATE_MIN && cp <= UTF8_LEAD_SURROGATE_MAX))
		{
			next = (*start++);
			if (swapEndian)
				next = ((next & 0xff) << 8 | ((next & 0xff00) >> 8));

			uint32_t trail = 0xffff & next;
			cp = (cp << 10) + trail + UTF8_SURROGATE_OFFSET;
		}

		append(cp);
	}

	return *this;
}

Str& Str::trim()
{
	const char* s = begin();
	const char* e = end() - 1;

	while (isspace(*s) && s != e)
		s++;
	while (isspace(*e) && s != e)
		e--;

	set(s, e + 1);
	return *this;
}

bool Str::starts_with(const char* str, bool ignoreCase) const
{
	int len = (int)strlen(str);
	if (len > m_length || len <= 0)
		return false;

	const char* a = data();
	const char* b = str;

	if (ignoreCase)
	{
		for (int n = 0; n < len; n++)
			if (tolower(a[n]) != tolower(b[n]))
				return false;
	}
	else
	{
		for (int n = 0; n < len; n++)
			if (a[n] != b[n])
				return false;
	}

	return true;
}

bool Str::contains(const char* str, bool ignoreCase) const
{
	int len = (int)strlen(str);
	if (len > m_length || len <= 0)
		return false;

	const char* a = data();
	const char* b = str;

	for (int start = 0; start < m_length - len + 1; start++)
	{
		bool match = true;

		if (ignoreCase)
		{
			for (int n = 0; n < len && match; n++)
				if (tolower(a[start + n]) != tolower(b[n]))
					match = false;
		}
		else
		{
			for (int n = 0; n < len && match; n++)
				if (a[start + n] != b[n])
					match = false;
		}

		if (match)
			return true;
	}

	return false;
}

bool Str::ends_with(const char* str, bool ignoreCase) const
{
	int len = (int)strlen(str);
	if (len > m_length || len <= 0)
		return false;

	const char* a = data();
	const char* b = str;

	if (ignoreCase)
	{
		for (int n = m_length - len, i = 0; n < m_length; n++, i++)
			if (tolower(a[n]) != tolower(b[i]))
				return false;
	}
	else
	{
		for (int n = m_length - len, i = 0; n < m_length; n++, i++)
			if (a[n] != b[i])
				return false;
	}

	return true;
}

int Str::first_index_of(char ch) const
{
	const char* ptr = data();
	for (int n = 0; n < m_length; n++)
		if (ptr[n] == ch)
			return n;
	return -1;
}

int Str::last_index_of(char ch) const
{
	const char* ptr = data();
	for (int n = m_length - 1; n >= 0; n--)
		if (ptr[n] == ch)
			return n;
	return -1;
}

String Str::substr(int start) const
{
	if (start < 0) start = 0;
	if (start > m_length) start = m_length;

	return String(data() + start);
}

String Str::substr(int start, int end) const
{
	if (start < 0)			start = 0;
	if (start > m_length)	start = m_length;
	if (end < 0)			end = m_length + end;
	if (end < start)		end = start;
	if (end > m_length)		end = m_length;

	return Str(data() + start, data() + end);
}

Vector<String> Str::split(char ch) const
{
	Vector<String> result;

	const char* ptr = data();
	int last = 0;
	int index = 1;

	while (index < m_length)
	{
		if (ptr[index] == ch)
		{
			result.push_back(substr(last, index));
			last = index + 1;
		}
		index++;
	}

	if (last < index)
		result.push_back(substr(last, index));

	return result;
}

Str& Str::replace(const Str& os, const Str& ns)
{
	for (int i = 0; i < m_length - os.m_length + 1; i++)
	{
		if (strcmp(data() + i, os.data()) == 0)
		{
			if (ns.m_length > os.m_length)
				reserve(ns.m_length - os.m_length);

			memcpy(data() + i + os.m_length, data() + i + ns.m_length, m_length - i - os.m_length);
			memcpy(data() + i, ns.cstr(), ns.m_length);
			set_length(m_length + ns.m_length - os.m_length);
			i += os.m_length - 1;
		}
	}

	return *this;
}

Str& Str::replace(char c, char r)
{
	char* ptr = data();
	for (int n = 0; n < m_length; n++)
		if (ptr[n] == c)
			ptr[n] = r;
	return *this;
}

void Str::clear()
{
	if (m_capacity > 0)
		data()[0] = '\0';
	m_length = 0;
}

void Str::dispose()
{
	if (m_buffer != nullptr && m_buffer != empty_buffer)
		delete[] m_buffer;

	if (m_local_size > 0)
	{
		m_buffer = nullptr;
		m_capacity = m_local_size;
		data()[0] = '\0';
	}
	else
	{
		m_buffer = empty_buffer;
		m_capacity = 0;
	}

	m_length = 0;
}

void Str::set(const char* start, const char* end)
{
	// find the end
	if (end == nullptr)
		end = start + strlen(start);

	// reserve
	m_length = (int)(end - start);
	reserve(m_length);

	// copy the data over
	char* ptr = data();
	memcpy(ptr, start, m_length);
	ptr[m_length] = '\0';
}