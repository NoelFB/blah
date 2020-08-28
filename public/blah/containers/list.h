#pragma once
#include <blah/log.h>
#include <type_traits>
#include <new>
#include <string.h>

namespace Blah
{
	template<class T>
	class List
	{
	private:
		// internal list buffer
		T* m_buffer;

		// total elements
		int m_count;

		// total capacity
		int m_capacity;

	public:

		List();
		List(int capacity);
		List(const List& src);
		List(List&& src) noexcept;
		~List();

		List& operator=(const List& src);
		List& operator=(List&& src) noexcept;

		// reserves the given amount of capacity
		void reserve(int new_capacity);

		// adds an element to the list
		void add(const T& item);

		template<class ...Args>
		void emplace(Args&&...args);

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

		// checks whether the given value is in the List (uses == operator)
		bool contains(const T& item) const;

		// returns the index of the given value in the list (uses == operator, -1 if not in list)
		int index_of(const T& item) const;

		// pops the top element off the list
		T pop();

		// returns the total number of elements
		int count() const { return m_count; }

		// returns the internal buffer capacity of the list
		int capacity() const { return m_capacity; }

		// removes all elements and disposes the internal buffer
		void dispose();
	};

	template<class T>
	List<T>::List()
	{
		m_buffer = nullptr;
		m_count = m_capacity = 0;
	}
	
	template<class T>
	List<T>::List(int capacity)
	{
		m_buffer = nullptr;
		m_count = m_capacity = 0;
		Reserve(m_capacity);
	}

	template<class T>
	List<T>::List(const List<T>& src)
	{
		m_buffer = (T*)(::operator new (sizeof(T) * src.m_capacity));
		m_count = src.m_count;
		m_capacity = src.m_capacity;

		for (int n = 0; n < m_count; n++)
			new (m_buffer + n) T(*(src.m_buffer + n));
	}

	template<class T>
	List<T>::List(List<T>&& src) noexcept
	{
		m_buffer = src.m_buffer;
		m_count = src.m_count;
		m_capacity = src.m_capacity;
		src.m_buffer = nullptr;
		src.m_count = src.m_capacity = 0;
	}

	template<class T>
	List<T>& List<T>::operator=(const List<T>& src)
	{
		clear();

		if (m_capacity < src.m_count)
			reserve(src.m_count);
		m_count = src.m_count;
	
		for (int n = 0; n < m_count; n++)
			new (m_buffer + n) T(*(src.m_buffer + n));

		return *this;
	}

	template<class T>
	List<T>& List<T>::operator=(List<T>&& src) noexcept
	{
		dispose();
		m_buffer = src.m_buffer;
		m_count = src.m_count;
		m_capacity = src.m_capacity;
		src.m_buffer = nullptr;
		src.m_count = src.m_capacity = 0;
		return *this;
	}

	template<class T>
	List<T>::~List()
	{
		dispose();
	}

	template<class T>
	void List<T>::reserve(int new_capacity)
	{
		if (new_capacity >= m_capacity)
		{
			auto last_capacity = m_capacity;

			if (m_capacity <= 0)
				m_capacity = 8;

			while (new_capacity >= m_capacity)
				m_capacity = m_capacity * 2;

			T* new_buffer = (T*)::operator new (sizeof(T) * m_capacity);
			
			if (std::is_trivially_copyable<T>::value)
			{
				memcpy(new_buffer, m_buffer, sizeof(T) * m_count);
			}
			else
			{
				for (int n = 0; n < m_count; n++)
					new (new_buffer + n) T(std::move(m_buffer[n]));
			}

			::operator delete (m_buffer, sizeof(T) * last_capacity);
			m_buffer = new_buffer;
		}
	}

	template<class T>
	T* List<T>::expand(int amount)
	{
		reserve(m_count + amount);
		for (int n = m_count; n < m_count + amount; n++)
			new (m_buffer + n) T();
		m_count += amount;
		return (m_buffer + m_count - amount);
	}

	template<class T>
	void List<T>::add(const T& item)
	{
		reserve(m_count + 1);
		new (m_buffer + m_count) T(item);
		m_count++;
	}

	template<class T>
	template<class ...Args>
	void List<T>::emplace(Args&&...args)
	{
		reserve(m_count + 1);
		new (m_buffer + m_count) T(args);
		m_count++;
	}

	template<class T>
	void List<T>::add(T&& item)
	{
		reserve(m_count + 1);
		new (m_buffer + m_count) T(std::move(item));
		m_count++;
	}

	template<class T>
	T& List<T>::operator[](int index)
	{
		BLAH_ASSERT(index >= 0 && index < m_count, "Index is out of range");
		return m_buffer[index];
	}

	template<class T>
	const T& List<T>::operator[](int index) const
	{
		BLAH_ASSERT(index >= 0 && index < m_count, "Index is out of range");
		return m_buffer[index];
	}

	template<class T>
	void List<T>::clear()
	{
		for (T* it = m_buffer; it < m_buffer + m_count; it++)
			it->~T();
		m_count = 0;
	}

	template<class T>
	void List<T>::remove_at(int index)
	{
		BLAH_ASSERT(index >= 0 && index < m_count, "Index is out of range");

		m_buffer[index].~T();

		if (index < m_count - 1)
		{
			int diff = (m_count - index - 1);
			if (diff <= 0)
				diff = 0;

			if (std::is_trivially_copyable<T>::value)
			{
				memmove(m_buffer + index, m_buffer + index + 1, (size_t)diff * sizeof(T));
			}
			else
			{
				for (int i = index; i < m_count - 1; i++)
					m_buffer[i] = std::move(m_buffer[i + 1]);
				m_buffer[m_count - 1].~T();
			}
		}

		m_count--;
	}

	template<class T>
	bool List<T>::contains(const T& item) const
	{
		for (int i = 0; i < m_count; i++)
			if (*(m_buffer + i) == item)
				return true;
		return false;
	}

	template<class T>
	int List<T>::index_of(const T& item) const
	{
		for (int i = 0; i < m_count; i++)
			if (*(m_buffer + i) == item)
				return i;
		return -1;
	}
	
	template<class T>
	T List<T>::pop()
	{
		BLAH_ASSERT(m_count > 0, "There are no elements to pop");
		m_count--;

		T item = m_buffer[m_count];
		m_buffer[m_count].~T();
		return item;
	}

	template<class T>
	void List<T>::dispose()
	{
		clear();
		::operator delete (m_buffer, sizeof(T) * m_capacity);
		m_buffer = nullptr;
		m_count = 0;
		m_capacity = 0;
	}

}
