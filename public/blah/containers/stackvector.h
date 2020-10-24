#pragma once
#include <blah/log.h>
#include <string.h>
#include <new>

namespace Blah
{
	// A mix between an std::vector and an std::array
	// The goal is to have an std::vector implementation that lives
	// on the stack instead of the heap
	template<class T, int Capacity>
	class StackVector
	{
	private:
		// we avoid using an array of type T to make sure the default
		// constructor/destructors are not called
		char m_buffer[Capacity * sizeof(T)];
		size_t m_size;

	public:

		StackVector();
		StackVector(const StackVector& src);
		StackVector(StackVector&& src) noexcept;
		~StackVector();

		StackVector& operator=(const StackVector& src);
		StackVector& operator=(StackVector&& src) noexcept;

		void push_back(const T& item);
		void push_back(T&& item);

		template<class ... Args>
		void emplace_back(Args&&... args) { push_back(T(std::forward<Args>(args)...)); }

		T& operator[](int index);
		const T& operator[](int index) const;

		T* begin() { return (T*)m_buffer; }
		const T* begin() const { return (T*)m_buffer; }
		T* end() { return ((T*)m_buffer) + m_size; }
		const T* end() const { return ((T*)m_buffer) + m_size; }
		T& front() { BLAH_ASSERT(m_size > 0, "Index out of range"); return *begin(); }
		const T& front() const { BLAH_ASSERT(m_size > 0, "Index out of range"); return *begin(); }
		T& back() { BLAH_ASSERT(m_size > 0, "Index out of range"); return *(end() - 1); }
		const T& back() const { BLAH_ASSERT(m_size > 0, "Index out of range"); return *(end() - 1); }

		void clear();

		void erase(const T* position);

		size_t size() const { return m_size; }
		constexpr size_t capacity() const { return Capacity; }
	};

	template<class T, int Capacity>
	StackVector<T, Capacity>::StackVector()
	{
		m_size = 0;
		m_buffer[0] = 0;
	}

	template<class T, int Capacity>
	StackVector<T, Capacity>::StackVector(const StackVector<T, Capacity>& src)
	{
		m_size = src.m_size;
		for (int n = 0; n < m_size; n++)
			new(begin() + n) T(*(src.begin() + n));
	}

	template<class T, int Capacity>
	StackVector<T, Capacity>::StackVector(StackVector<T, Capacity>&& src) noexcept
	{
		m_size = src.m_size;

		if (std::is_trivially_copyable<T>::value)
		{
			memcpy(m_buffer, src.m_buffer, sizeof(T) * m_size);
			src.m_size = 0;
		}
		else
		{
			for (int n = 0; n < m_size; n++)
				new(begin() + n) T(std::move(*(src.begin() + n)));
			src.clear();
		}
	}

	template<class T, int Capacity>
	StackVector<T, Capacity>& StackVector<T, Capacity>::operator=(const StackVector<T, Capacity>& src)
	{
		clear();

		m_size = src.m_size;
		for (int n = 0; n < m_size; n++)
			new(begin() + n) T(*(src.begin() + n));

		return *this;
	}

	template<class T, int Capacity>
	StackVector<T, Capacity>& StackVector<T, Capacity>::operator=(StackVector<T, Capacity>&& src) noexcept
	{
		clear();

		m_size = src.m_size;

		if (std::is_trivially_copyable<T>::value)
		{
			memcpy(m_buffer, src.m_buffer, sizeof(T) * m_size);
			src.m_size = 0;
		}
		else
		{
			for (int n = 0; n < m_size; n++)
				new(begin() + n) T(std::move(*(src.begin() + n)));
			src.clear();
		}

		return *this;
	}

	template<class T, int Capacity>
	StackVector<T, Capacity>::~StackVector()
	{
		clear();
	}

	template<class T, int Capacity>
	void StackVector<T, Capacity>::push_back(const T& item)
	{
		BLAH_ASSERT(m_size < Capacity, "Exceeded StackVector Capacity");
		new(begin() + m_size) T(item);
		m_size++;
	}

	template<class T, int Capacity>
	void StackVector<T, Capacity>::push_back(T&& item)
	{
		BLAH_ASSERT(m_size < Capacity, "Exceeded StackVector Capacity");
		new(begin() + m_size) T(std::move(item));
		m_size++;
	}

	template<class T, int Capacity>
	T& StackVector<T, Capacity>::operator[](int index)
	{
		BLAH_ASSERT(index >= 0 && index < m_size, "Index is out of range");
		return begin()[index];
	}

	template<class T, int Capacity>
	const T& StackVector<T, Capacity>::operator[](int index) const
	{
		BLAH_ASSERT(index >= 0 && index < m_size, "Index is out of range");
		return begin()[index];
	}



	template<class T, int Capacity>
	void StackVector<T, Capacity>::clear()
	{
		for (T* it = begin(); it < begin() + m_size; it++)
			it->~T();
		m_size = 0;
	}

	template<class T, int Capacity>
	void StackVector<T, Capacity>::erase(const T* position)
	{
		BLAH_ASSERT(position >= begin() && position < end(), "Index is out of range");

		const size_t index = position - begin();
		if (index < m_size - 1)
		{
			size_t diff = (m_size - index - 1);
			if (diff <= 0) diff = 0;

			if (std::is_trivially_copyable<T>::value)
			{
				begin()[index].~T();
				memmove(begin() + index, begin() + index + 1, (size_t)diff * sizeof(T));
			}
			else
			{
				for (auto i = index; i < m_size - 1; i++)
					begin()[i] = std::move(begin()[i + 1]);
				begin()[m_size - 1].~T();
			}
		}
		else
		{
			begin()[index].~T();
		}

		m_size--;
	}

}
