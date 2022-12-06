#pragma once
#include <blah_common.h>
#include <blah_vector.h>
#include <blah_stackvector.h>

namespace Blah
{
	class BaseString
	{
	public:
		const char* cstr() const { return s_ptr(); }
		char* cstr() { return s_ptr(); }
		const char* begin() const { return s_ptr(); }
		char* begin() { return s_ptr(); }
		const char* end() const { return s_ptr() + length(); }
		char* end() { return s_ptr() + length(); }

		char& operator[](int index)
		{
			BLAH_ASSERT(index >= 0 && index < length() + 1, "Index out of range");
			return s_ptr()[index];
		}

		const char& operator[](int index) const
		{
			BLAH_ASSERT(index >= 0 && index < length() + 1, "Index out of range");
			return s_ptr()[index];
		}

		operator char* () { return cstr(); }
		operator const char* () const { return cstr(); }

		void assign(const char* cstr, const char* cstr_end = nullptr);
		void append(const char* cstr, const char* cstr_end = nullptr);
		void append(const u16* u16_cstr, const u16* u16_cstr_end = nullptr, bool swap_endian = false);
		void append(char ch, int count = 1);
		void append(u32 unicode);
		void append_fmt(const char* fmt, ...);

		bool starts_with(const char* cstr, bool ignore_case = false) const;
		bool ends_with(const char* cstr, bool ignore_case = false) const;
		bool contains(const char* cstr, bool ignore_case = false) const;

		int first_index_of(char ch) const;
		int last_index_of(char ch) const;

		int length() const
		{
			int n = s_len() - 1;    // reduce by 1 for null-terminator
			return (n > 0 ? n : 0); // safety, although due to how StackString is implemented s_len shouldn't ever be less than 1
		}

		bool equals(const char* other, bool ignore_case = false) const;
		bool empty() const { return length() == 0; }
		void clear() { s_clear(); }

		bool operator==(const char* rhs) const { return equals(rhs); }
		bool operator!=(const char* rhs) const { return !(*this == rhs); }
		bool operator==(const BaseString& rhs) const { return s_len() == rhs.s_len() && *this == rhs.cstr(); }
		bool operator!=(const BaseString& rhs) const { return !(*this == rhs); }

	protected:
		virtual void s_clear() = 0;
		virtual void s_ensure(int capacity) = 0;
		virtual char* s_ptr() = 0;
		virtual const char* s_ptr() const = 0;
		virtual int s_len() const = 0;
	};

	template<size_t StackSize>
	class StackString final : public BaseString
	{
	public:
		StackString() { assign(""); }
		StackString(const char* cstr, const char* cstr_end = nullptr) { assign(cstr, cstr_end); }
		StackString(const BaseString& other) { assign(other.cstr()); }

		StackString& operator=(const BaseString& rhs)
		{
			assign(rhs.cstr(), rhs.cstr() + rhs.length());
			return *this;
		}
		
		StackString& operator=(const char* rhs)
		{
			assign(rhs, nullptr);
			return *this;
		}
		
		StackString& operator+=(const BaseString& rhs)
		{
			append(rhs.cstr());
			return *this;
		}
		
		StackString& operator+=(const char* rhs)
		{
			append(rhs);
			return *this;
		}

		StackString& operator+=(const char& rhs)
		{
			append(rhs);
			return *this;
		}

		StackString substr(int start, int len = 0) const
		{
			if (len == 0) len = length() - start;
			return StackString(cstr() + start, cstr() + start + len);
		}

		StackString replace(const StackString& term, const StackString& replacement) const
		{
			StackString result;
			int term_length = term.length();
			for (int i = 0, n = length() - term_length; i < n; i ++)
			{
				bool match = false;
				for (int j = 0; j < term_length; j ++)
					if (operator[](i) != term[j]) { match = true; break; }
				if (match)
				{
					result.append(replacement);
					i += term_length - 1;
				}
				else
					result.append(operator[](i));
			}
			return result;
		}

		void set_length(int new_length)
		{
			new_length++;
			if (s_len() < new_length)
				append('\0', new_length - s_len());
			else if (m_heap_buffer.size() > 0)
				m_heap_buffer.erase(new_length, s_len() - new_length);
			else
				m_stack_buffer.erase(new_length, s_len() - new_length);
		}

		StackString trim() const
		{
			if (length() > 0)
			{
				const char* s = begin();
				const char* e = end() - 1;

				while (s < e && (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n' || *s == '\v' || *s == '\f')) s++;
				while (e > s && (*e == ' ' || *e == '\t' || *e == '\r' || *e == '\n' || *e == '\v' || *e == '\f')) e--;

				return StackString(s, e + 1);
			}

			return StackString();
		}

		Vector<StackString> split(char ch) const
		{
			Vector<StackString> result;

			const char* ptr = s_ptr();
			const char* end = ptr + length();
			const char* last = ptr;

			while (ptr < end)
			{
				if (*ptr == ch)
				{
					result.emplace_back(last, ptr);
					last = ptr + 1;
				}
				ptr++;
			}

			if (last < ptr)
				result.emplace_back(last, ptr);

			return result;
		}

		static StackString fmt(const char* fmt, ...)
		{
			StackString str;

			va_list args;

			// determine arg m_length
			va_start(args, fmt);
			auto add = vsnprintf(nullptr, 0, fmt, args);
			va_end(args);

			if (add <= 0)
				return str;

			// reserve (+1 for null-terminator)
			auto len = str.length();
			str.s_ensure(len + add + 1);

			// print out
			va_start(args, fmt);
			vsnprintf(str.s_ptr() + len, add + 1, fmt, args);
			va_end(args);

			return str;
		}

	protected:
		void s_clear() override
		{
			m_stack_buffer.clear();
			m_heap_buffer.clear();
			s_ensure(1);
		}

		void s_ensure(int capacity) override
		{
			int count = capacity - s_len();
			if (count <= 0)
				return;

			// expand heap buffer
			if (m_heap_buffer.size() > 0)
			{
				m_heap_buffer.expand(count);
			}
			// switch from stack to heap
			else if (capacity > StackSize)
			{
				m_heap_buffer.expand(capacity);

				char* src = m_stack_buffer.data();
				char* len = src + m_stack_buffer.size();
				char* dst = m_heap_buffer.data();
				while (src < len) *(dst++) = *(src++);
			}
			// expand stack buffer
			else
			{
				m_stack_buffer.expand(count);
			}

			*(s_ptr() + s_len() - 1) = '\0';
		}

		char* s_ptr() override
		{
			return m_heap_buffer.size() > 0 ? m_heap_buffer.data() : m_stack_buffer.data();
		}

		const char* s_ptr() const override
		{
			return m_heap_buffer.size() > 0 ? m_heap_buffer.data() : m_stack_buffer.data();
		}

		int s_len() const override
		{
			return m_heap_buffer.size() > 0 ? m_heap_buffer.size() : m_stack_buffer.size();
		}

	private:
		Vector<char> m_heap_buffer;
		StackVector<char, StackSize> m_stack_buffer;
	};

	template<size_t StackSizeLeft, size_t StackSizeRight>
	StackString<StackSizeLeft> operator+(const StackString<StackSizeLeft>& lhs, const StackString<StackSizeRight>& rhs)
	{
		StackString<StackSizeLeft> str(lhs); str.append(rhs); return str;
	}

	template<size_t StackSize>
	StackString<StackSize> operator+(const char* lhs, const StackString<StackSize>& rhs)
	{
		StackString<StackSize> str(lhs); str.append(rhs); return str;
	}

	template<size_t StackSize>
	StackString<StackSize> operator+(const StackString<StackSize>& lhs, const char* rhs)
	{
		StackString<StackSize> str(lhs); str.append(rhs); return str;
	}
	
	template<size_t StackSize>
	StackString<StackSize> operator+(const StackString<StackSize>& lhs, const char& rhs)
	{
		StackString<StackSize> str(lhs); str.append(rhs); return str;
	}
	
	template<size_t StackSize>
	StackString<StackSize> operator+(const char& lhs, const StackString<StackSize>& rhs)
	{
		StackString<StackSize> str(lhs); str.append(rhs); return str;
	}


	// Stores enough for an empty string on the stack, and afterwards allocates on the heap
	using HeapString = StackString<1>;

	// Standard String with a fair amount of stack storage
	using String = StackString<64>;

	// Large String with enough stack storage to store FilePaths without allocating.
	using FilePath = StackString<260>;

	// Utf8 Utility, to iterate over a string and read utf-8 characters
	struct Utf8
	{
		const char* str = nullptr;
		u32 character = 0;
		u32 character_size = 0;

		Utf8() = default;
		Utf8(const char* cstr);
		
		// moves to the next character, returns false if at end of string or an invalid character
		bool next();
	};

	struct CaseInsenstiveStringHash
	{
		std::size_t operator()(const Blah::BaseString& key) const
		{
			std::size_t result = 2166136261U;

			for (auto& it : key)
			{
				if (it >= 'A' && it <= 'Z')
					result ^= (static_cast<std::size_t>(it) - 'A' + 'a');
				else
					result ^= static_cast<std::size_t>(it);
				result *= 16777619U;
			}

			return result;
		}
	};

	struct CaseInsenstiveStringCompare
	{
		bool operator() (const BaseString& lhs, const BaseString& rhs) const
		{
			return lhs.length() == rhs.length() && lhs.starts_with(rhs, true);
		}
	};
}

// STD Hash Utility, so the String can be used in a set / unordered_map / map

namespace std
{
	template <>
	struct hash<Blah::BaseString>
	{
		std::size_t operator()(const Blah::BaseString& key) const
		{
			std::size_t result = 2166136261U;

			for (auto& it : key)
			{
				result ^= static_cast<std::size_t>(it);
				result *= 16777619U;
			}

			return result;
		}
	};

	template <size_t StackSize>
	struct hash<Blah::StackString<StackSize>>
	{
		std::size_t operator()(const Blah::StackString<StackSize>& key) const
		{
			std::size_t result = 2166136261U;

			for (auto& it : key)
			{
				result ^= static_cast<std::size_t>(it);
				result *= 16777619U;
			}

			return result;
		}
	};
}