#pragma once
#include <blah/log.h>

namespace Blah
{
	template<class T, int Size>
	class StackList
	{
	private:
		// internal list buffer
		T m_buffer[Size];

		// total elements
		int m_count;

	public:

		StackList();
		StackList(const StackList& src);
		StackList(StackList&& src) noexcept;

		StackList& operator=(const StackList& src);
		StackList& operator=(StackList&& src) noexcept;

		// adds an element to the list
		void add(const T& item);

		// moves an element into the list
		void add(T&& item);

		// expands the list by the given amount, and returns a pointer to the first element
		T* expand(int amount = 1);

		// returns a reference to the element at the given index
		T& operator[](int index);

		// returns a reference to the element at the given index
		const T& operator[](int index) const;

		// returns a pointer to the first element
		T* begin() { return m_buffer; }

		// returns a pointer to the first element
		const T* begin() const { return m_buffer; }

		// returns a pointer to the last element
		T* end() { return m_buffer + m_count; }

		// returns a pointer to the last element
		const T* end() const { return m_buffer + m_count; }

		// clears all elements
		void clear();

		// removes the element at the index
		void remove_at(int index);

		// checks whether the given value is in the StackList (uses == operator)
		bool contains(const T& item) const;

		// returns the index of the given value in the list (uses == operator, -1 if not in list)
		int index_of(const T& item) const;

		// pops the top element off the list
		T& pop();

		// returns the total number of elements
		int count() const { return m_count; }

		// returns the internal buffer capacity of the list
		int capacity() const { return Size; }
	};

	template<class T, int Size>
	StackList<T, Size>::StackList()
	{
		m_count = 0;
	}

	template<class T, int Size>
	StackList<T, Size>::StackList(const StackList<T, Size>& src)
	{
		m_count = src.m_count;

		for (int n = 0; n < m_count; n++)
			m_buffer[n] = src.m_buffer[n];
	}

	template<class T, int Size>
	StackList<T, Size>::StackList(StackList<T, Size>&& src) noexcept
	{
		m_count = src.m_count;
		for (int n = 0; n < m_count; n++)
			m_buffer[n] = std::move(src.m_buffer[n]);
		src.m_count = 0;
	}

	template<class T, int Size>
	StackList<T, Size>& StackList<T, Size>::operator=(const StackList<T, Size>& src)
	{
		clear();

		m_count = src.m_count;
		for (int n = 0; n < m_count; n++)
			m_buffer[n] = src.m_buffer[n];

		return *this;
	}

	template<class T, int Size>
	StackList<T, Size>& StackList<T, Size>::operator=(StackList<T, Size>&& src) noexcept
	{
		clear();

		m_count = src.m_count;
		for (int n = 0; n < m_count; n++)
			m_buffer[n] = std::move(src.m_buffer[n]);
		src.m_count = 0;

		return *this;
	}

	template<class T, int Size>
	T* StackList<T, Size>::expand(int amount)
	{
		BLAH_ASSERT(m_count + amount < Size, "Exceeded StackList capacity");

		for (int n = m_count; n < m_count + amount; n++)
			m_buffer[n] = T();
		m_count += amount;

		return (m_buffer + m_count - amount);
	}

	template<class T, int Size>
	void StackList<T, Size>::add(const T& item)
	{
		BLAH_ASSERT(m_count < Size, "Exceeded StackList capacity");
		m_buffer[m_count] = item;
		m_count++;
	}

	template<class T, int Size>
	void StackList<T, Size>::add(T&& item)
	{
		BLAH_ASSERT(m_count < Size, "Exceeded StackList capacity");
		m_buffer[m_count] = std::move(item);
		m_count++;
	}

	template<class T, int Size>
	T& StackList<T, Size>::operator[](int index)
	{
		BLAH_ASSERT(index >= 0 && index < m_count, "Index is out of range");
		return m_buffer[index];
	}

	template<class T, int Size>
	const T& StackList<T, Size>::operator[](int index) const
	{
		BLAH_ASSERT(index >= 0 && index < m_count, "Index is out of range");
		return m_buffer[index];
	}

	template<class T, int Size>
	void StackList<T, Size>::clear()
	{
		m_count = 0;
	}

	template<class T, int Size>
	void StackList<T, Size>::remove_at(int index)
	{
		BLAH_ASSERT(index >= 0 && index < m_count, "Index is out of range");

		if (index < m_count - 1)
		{
			for (int n = index; n < m_count - 1; n++)
				m_buffer[n] = m_buffer[n + 1];
			m_buffer[m_count] = T();
		}

		m_count--;
	}

	template<class T, int Size>
	bool StackList<T, Size>::contains(const T& item) const
	{
		for (int i = 0; i < m_count; i++)
			if (*(m_buffer + i) == item)
				return true;
		return false;
	}

	template<class T, int Size>
	int StackList<T, Size>::index_of(const T& item) const
	{
		for (int i = 0; i < m_count; i++)
			if (*(m_buffer + i) == item)
				return i;
		return -1;
	}

	template<class T, int Size>
	T& StackList<T, Size>::pop()
	{
		BLAH_ASSERT(m_count > 0, "There are no elements to pop");
		m_count--;
		return m_buffer[m_count];
	}

}
