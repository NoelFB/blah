#pragma once
#include <blah/log.h>
#include <type_traits>
#include <new>
#include <string.h>

namespace Blah
{
	template<class T>
	class Vector
	{
	private:
		T* m_buffer;
		size_t m_size;
		size_t m_capacity;

	public:

		Vector();
		Vector(int capacity);
		Vector(const Vector& src);
		Vector(Vector&& src) noexcept;
		~Vector();

		Vector& operator=(const Vector& src);
		Vector& operator=(Vector&& src) noexcept;

		// reserves the given amount of capacity
		void reserve(size_t new_capacity);

		// adds an element to the Vector
		void push_back(const T& item);

		// moves an element into the Vector
		void push_back(T&& item);

		template<class ...Args>
		void emplace_back(Args&&...args);

		// resizes the vector
		void resize(size_t capacity);

		// expands the Vector by the given amount, and returns a pointer to the first element
		T* expand(size_t amount = 1);

		// returns a reference to the element at the given index
		T& operator[](int index);

		// returns a reference to the element at the given index
		const T& operator[](int index) const;

		// returns a pointer to the first element
		T* data() { return m_buffer; }

		// returns a pointer to the first element
		const T* data() const { return m_buffer; }

		// returns a pointer to the first element
		T* begin() { return m_buffer; }

		// returns a pointer to the first element
		const T* begin() const { return m_buffer; }

		// returns a pointer to the last element plus 1
		T* end() { return m_buffer + m_size; }

		// returns a pointer to the last element plus 1
		const T* end() const { return m_buffer + m_size; }

		// returns the front element
		T& front() { BLAH_ASSERT(m_size > 0, "Index out of range"); return *begin(); }
		
		// returns the front element
		const T& front() const { BLAH_ASSERT(m_size > 0, "Index out of range"); return *begin(); }

		// returns the last element
		T& back() { BLAH_ASSERT(m_size > 0, "Index out of range"); return *(end() - 1); }

		// returns the last element
		const T& back() const { BLAH_ASSERT(m_size > 0, "Index out of range"); return *(end() - 1); }

		// clears all elements
		void clear();

		// removes the element at the index
		void erase(const T* position);

		// returns the total number of elements
		size_t size() const { return m_size; }

		// returns the internal buffer capacity of the Vector
		size_t capacity() const { return m_capacity; }

		// removes all elements and disposes the internal buffer
		void dispose();
	};

	template<class T>
	Vector<T>::Vector()
	{
		m_buffer = nullptr;
		m_size = m_capacity = 0;
	}

	template<class T>
	Vector<T>::Vector(int capacity)
	{
		m_buffer = nullptr;
		m_size = m_capacity = capacity;
		reserve(m_capacity);
	}

	template<class T>
	Vector<T>::Vector(const Vector<T>& src)
	{
		m_buffer = (T*)(::operator new (sizeof(T) * src.m_capacity));
		m_size = src.m_size;
		m_capacity = src.m_capacity;

		for (int n = 0; n < m_size; n++)
			new (m_buffer + n) T(*(src.m_buffer + n));
	}

	template<class T>
	Vector<T>::Vector(Vector<T>&& src) noexcept
	{
		m_buffer = src.m_buffer;
		m_size = src.m_size;
		m_capacity = src.m_capacity;
		src.m_buffer = nullptr;
		src.m_size = src.m_capacity = 0;
	}

	template<class T>
	Vector<T>& Vector<T>::operator=(const Vector<T>& src)
	{
		clear();

		if (m_capacity < src.m_size)
			reserve(src.m_size);
		m_size = src.m_size;

		for (int n = 0; n < m_size; n++)
			new (m_buffer + n) T(*(src.m_buffer + n));

		return *this;
	}

	template<class T>
	Vector<T>& Vector<T>::operator=(Vector<T>&& src) noexcept
	{
		dispose();
		m_buffer = src.m_buffer;
		m_size = src.m_size;
		m_capacity = src.m_capacity;
		src.m_buffer = nullptr;
		src.m_size = src.m_capacity = 0;
		return *this;
	}

	template<class T>
	Vector<T>::~Vector()
	{
		dispose();
	}

	template<class T>
	void Vector<T>::reserve(size_t new_capacity)
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
				memcpy(new_buffer, m_buffer, sizeof(T) * m_size);
			}
			else
			{
				for (int n = 0; n < m_size; n++)
				{
					new (new_buffer + n) T(std::move(m_buffer[n]));
					m_buffer[n].~T();
				}
			}

			::operator delete (m_buffer, sizeof(T) * last_capacity);
			m_buffer = new_buffer;
		}
	}

	template<class T>
	void Vector<T>::resize(size_t new_size)
	{
		if (new_size > m_size)
		{
			reserve(new_size);
			for (int n = m_size; n < new_size; n++)
				new (m_buffer + n) T();
		}
		else if (new_size < m_size)
		{
			for (int i = new_size; i < m_size; i ++)
				m_buffer[i].~T();
		}

		m_size = new_size;
	}

	template<class T>
	T* Vector<T>::expand(size_t amount)
	{
		reserve(m_size + amount);
		for (int n = m_size; n < m_size + amount; n++)
			new (m_buffer + n) T();
		m_size += amount;
		return (m_buffer + m_size - amount);
	}

	template<class T>
	void Vector<T>::push_back(const T& item)
	{
		reserve(m_size + 1);
		new (m_buffer + m_size) T(item);
		m_size++;
	}

	template<class T>
	void Vector<T>::push_back(T&& item)
	{
		reserve(m_size + 1);
		new (m_buffer + m_size) T(std::move(item));
		m_size++;
	}

	template<class T>
	template<class ...Args>
	void Vector<T>::emplace_back(Args&&...args)
	{
		reserve(m_size + 1);
		new (m_buffer + m_size) T(std::forward<Args>(args)...);
		m_size++;
	}

	template<class T>
	T& Vector<T>::operator[](int index)
	{
		BLAH_ASSERT(index >= 0 && index < m_size, "Index is out of range");
		return m_buffer[index];
	}

	template<class T>
	const T& Vector<T>::operator[](int index) const
	{
		BLAH_ASSERT(index >= 0 && index < m_size, "Index is out of range");
		return m_buffer[index];
	}

	template<class T>
	void Vector<T>::clear()
	{
		for (T* it = m_buffer; it < m_buffer + m_size; it++)
			it->~T();
		m_size = 0;
	}

	template<class T>
	void Vector<T>::erase(const T* position)
	{
		BLAH_ASSERT(position >= begin() && position < end(), "Index is out of range");

		const size_t index = position - begin();

		if (index < m_size - 1)
		{
			size_t diff = (m_size - index - 1);
			if (diff <= 0) diff = 0;

			if (std::is_trivially_copyable<T>::value)
			{
				m_buffer[index].~T();
				memmove(m_buffer + index, m_buffer + index + 1, (size_t)diff * sizeof(T));
			}
			else
			{
				for (int i = index; i < m_size - 1; i++)
					m_buffer[i] = std::move(m_buffer[i + 1]);
				m_buffer[m_size - 1].~T();
			}
		}
		else
		{
			m_buffer[index].~T();
		}

		m_size--;
	}

	template<class T>
	void Vector<T>::dispose()
	{
		clear();
		::operator delete (m_buffer, sizeof(T) * m_capacity);
		m_buffer = nullptr;
		m_size = 0;
		m_capacity = 0;
	}

}
