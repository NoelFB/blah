#pragma once
#include <inttypes.h>
#include <stdarg.h>
#include <cstdio>
#include <blah/containers/vector.h>

namespace Blah
{
	template<int T>
	class StrOf;
	using String = StrOf<64>;

	class Str
	{
	public:
		Str() { m_buffer = empty_buffer; m_length = m_capacity = m_local_size = 0; }
		Str(const char* start, const char* end = nullptr) : Str() { set(start, end); }
		Str(const Str& str) : Str() { set(str); }
		
		char& operator[](int index) { return data()[index]; }
		const char& operator[](int index) const { return data()[index]; }

		// equality operators
		bool operator==(const Str& rhs) const;
		bool operator!=(const Str& rhs) const;
		bool operator==(const char* rhs) const;
		bool operator!=(const char* rhs) const;

		// assignment operator
		Str& operator=(const Str& rhs) { set(rhs.cstr(), rhs.cstr() + rhs.m_length); return *this; }
		Str& operator=(const char* rhs) { set(rhs, nullptr); return *this; }

		// append string
		Str& operator+=(const Str& rhs) { return append(rhs); }

		// append cstr
		Str& operator+=(const char* rhs) { return append(rhs); }
		
		// combine string
		Str operator+(const Str& rhs) { Str str; str.append(*this).append(rhs); return str; }

		// combine cstr
		Str operator+(const char* rhs) { Str str; str.append(*this).append(rhs); return str; }

		// implicit cast to cstr
		operator char* () { return cstr(); }

		// implicit cast to cstr
		operator const char* () const { return cstr(); }

		// returns a pointer to the null-terminated string buffer
		char* cstr() { return data(); }

		// returns a pointer to the null-terminated string buffer
		const char* cstr() const { return data(); }

		// returns a pointer to the start of the buffer
		const char* begin() const { return data(); }

		// returns a pointer to the end of the buffer
		const char* end() const { return data() + m_length; }

		// returns the length of the string
		int length() const { return m_length; }

		// returns the capacity of the string
		int capacity() const { return m_capacity; }

		// returns the capacity of the string's stack buffer
		int stack_capacity() const { return m_local_size; }

		// sets the length of the string.
		// this does not set the value of the string!
		void set_length(int length);

		// ensures the string has the given capacity
		void reserve(int capacity);
		
		// Returns the unicode value at the given index.
		// Assumes the index is a valid utf8 starting point.
		uint32_t utf8_at(int index) const;

		// Returns the byte-length of the utf8 character.
		// Assumes the index is a valid utf8 starting point.
		int utf8_length(int index) const;

		// appends the given character
		Str& append(char c);

		// appends the given unicode character
		Str& append(uint32_t c);

		// appends the given c string
		Str& append(const char* start, const char* end = nullptr);

		// appends the given string
		Str& append(const Str& str, int start = 0, int end = -1);

		// appends the given formatted string
		Str& append_fmt(const char* fmt, ...);

		// appends a utf16 string
		Str& append_utf16(const uint16_t* start, const uint16_t* end = nullptr, bool swapEndian = false);

		// trims whitespace
		Str& trim();

		// returns true if the string begins with the given string
		bool starts_with(const Str& str, bool ignore_case = false) const { return starts_with(str.cstr(), ignore_case); }

		// returns true if the string begins with the given string
		bool equals(const Str& str, bool ignore_case = false) const { return str.length() == length() && starts_with(str.cstr(), ignore_case); }

		// returns true if the string begins with the given string
		bool starts_with(const char* str, bool ignore_case = false) const;

		// returns true if the string contains with the given string
		bool contains(const Str& str, bool ignore_case = false) const { return contains(str.cstr(), ignore_case); }

		// returns true if the string contains with the given string
		bool contains(const char* str, bool ignore_case = false) const;

		// returns true if the string ends with the given string
		bool ends_with(const Str& str, bool ignore_case = false) const { return ends_with(str.cstr(), ignore_case); }

		// returns true if the string ends with the given string
		bool ends_with(const char* str, bool ignore_case = false) const;

		// returns the first index of the given character, or -1 if it isn't found
		int first_index_of(char ch) const;

		// returns the last index of the given character, or -1 if it isn't found
		int last_index_of(char ch) const;

		// returns a substring of the string
		String substr(int start) const;

		// returns a substring of the string
		String substr(int start, int end) const;

		Vector<String> split(char ch) const;

		Str& replace(const Str& old_str, const Str& new_str);

		// replaces all occurances of the given character in the string
		Str& replace(char c, char r);

		// checks if the string has a length of 0
		bool empty() const { return m_length <= 0; }

		// clears the string length to 0
		void clear();

		// clears and disposes the internal string buffer
		void dispose();

		~Str()
		{
			if (m_buffer != nullptr && m_buffer != empty_buffer)
				delete[] m_buffer;
		}

	protected:
		Str(int local_size)
		{
			m_buffer = nullptr;
			m_length = 0;
			m_capacity = local_size;
			m_local_size = local_size;
		}

		// returns a pointer to the heap buffer or to our stack allocation
		char* data()			 { return (m_buffer != nullptr ? m_buffer : ((char*)(this) + sizeof(Str))); }
		const char* data() const { return (m_buffer != nullptr ? m_buffer : ((char*)(this) + sizeof(Str))); }

		void set(const Str& str) { set(str.cstr(), str.cstr() + str.m_length); }
		void set(const char* start, const char* end = nullptr);

	private:
		static char empty_buffer[1];
		char*	m_buffer;
		int		m_length;
		int		m_capacity;
		int		m_local_size;
	};

	// combine string
	inline Str operator+(const Str& lhs, const Str& rhs) { Str str; str.append(lhs).append(rhs); return str; }

	template<int T>
	class StrOf : public Str
	{
	private:
		char m_local_buffer[T];

	public:
		StrOf() : Str(T) { m_local_buffer[0] = '\0'; }
		StrOf(const char* rhs, const char* end = nullptr) : Str(T) { m_local_buffer[0] = '\0'; set(rhs, end); }
		StrOf(const Str& rhs) : Str(T) { m_local_buffer[0] = '\0'; set(rhs); }
		StrOf(const StrOf& rhs) : Str(T) { m_local_buffer[0] = '\0'; set(rhs); }

		// assignment operators
		StrOf& operator=(const char* rhs)	{ set(rhs); return *this; }
		StrOf& operator=(const Str& rhs)	{ set(rhs); return *this; }
		StrOf& operator=(const StrOf& rhs) { set(rhs); return *this; }

		// creates a string from the format
		static StrOf fmt(const char* str, ...);
	};

	template<int T>
	StrOf<T> StrOf<T>::fmt(const char* fmt, ...)
	{
		StrOf<T> str;

		int add, diff;

		// determine arg length
		va_list args;
		va_start(args, fmt);
		add = vsnprintf(NULL, 0, fmt, args);
		va_end(args);

		// reserve
		auto len = str.length();
		str.set_length(len + add);
		diff = str.capacity() - len;
		if (diff <= 0) diff = 0;

		// print out
		va_start(args, fmt);
		vsnprintf(str.cstr() + len, (size_t)diff, fmt, args);
		va_end(args);

		return str;
	}
}