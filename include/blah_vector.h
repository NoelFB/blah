#pragma once
#include <blah_common.h>

namespace Blah
{
	// A lightweight Vector implementation
	template<class T>
	class Vector
	{
	private:
		T* m_buffer;
		int m_count;
		int m_capacity;

	public:

		Vector();
		Vector(int capacity);
		Vector(const InitializerList<T>& list);
		Vector(const Vector& src);
		Vector(Vector&& src) noexcept;
		~Vector();

		Vector& operator=(const Vector& src);
		Vector& operator=(Vector&& src) noexcept;

		void clear();
		void dispose();

		int size() const;
		int capacity() const;

		void reserve(int new_capacity);
		void resize(int new_count);
		T* expand(int amount = 1);

		void push_back(const T& item);
		void push_back(T&& item);
		template<class ...Args>
		void emplace_back(Args&&...args);

		T& operator[](int index);
		const T& operator[](int index) const;

		T* data();
		const T* data() const;
		T* begin();
		const T* begin() const;
		T* end();
		const T* end() const;
		T& front();
		const T& front() const;
		T& back();
		const T& back() const;

		void erase(int index, int elements = 1);
		T pop();
	};


	template<class T>
	inline Vector<T>::Vector()
	{
		m_buffer = nullptr;
		m_count = m_capacity = 0;
	}

	template<class T>
	inline Vector<T>::Vector(int capacity)
	{
		m_buffer = nullptr;
		m_count = m_capacity = 0;
		reserve(capacity);
	}

	template<class T>
	inline Vector<T>::Vector(const Vector& src)
	{
		m_buffer = nullptr;
		m_count = m_capacity = 0;
		reserve(src.m_capacity);
		for (int i = 0; i < src.m_count; i++)
			new (m_buffer + i) T(src.m_buffer[i]);
		m_count = src.m_count;
	}

	template<class T>
	inline Vector<T>::Vector(Vector&& src) noexcept
	{
		m_buffer = src.m_buffer;
		m_capacity = src.m_capacity;
		m_count = src.m_count;
		src.m_buffer = nullptr;
		src.m_capacity = 0;
		src.m_count = 0;
	}

	template<class T>
	inline Vector<T>::Vector(const InitializerList<T>& list)
	{
		m_buffer = nullptr;
		m_count = m_capacity = 0;
		reserve((int)list.size());
		for (auto& it : list)
			push_back(std::move(it));
	}

	template<class T>
	inline Vector<T>::~Vector()
	{
		dispose();
	}

	template<class T>
	inline Vector<T>& Vector<T>::operator=(const Vector& src)
	{
		clear();
		reserve(src.m_capacity);
		for (int i = 0; i < src.m_count; i++)
			new (m_buffer + i) T(src.m_buffer[i]);
		m_count = src.m_count;
		return *this;
	}

	template<class T>
	inline Vector<T>& Vector<T>::operator=(Vector&& src) noexcept
	{
		dispose();
		m_buffer = src.m_buffer;
		m_capacity = src.m_capacity;
		m_count = src.m_count;
		src.m_buffer = nullptr;
		src.m_capacity = 0;
		src.m_count = 0;
		return *this;
	}

	template<class T>
	inline void Vector<T>::clear()
	{
		for (int i = 0; i < m_count; i++)
			m_buffer[i].~T();
		m_count = 0;
	}

	template<class T>
	inline void Vector<T>::dispose()
	{
		clear();

		::operator delete (m_buffer, sizeof(T) * m_capacity);

		m_capacity = 0;
		m_buffer = nullptr;
	}

	template<class T>
	inline int Vector<T>::size() const
	{
		return m_count;
	}

	template<class T>
	inline int Vector<T>::capacity() const
	{
		return m_capacity;
	}

	template<class T>
	inline void Vector<T>::reserve(int cap)
	{
		if (cap > m_capacity)
		{
			int new_capacity = m_capacity;
			if (new_capacity <= 0)
				new_capacity = 8;
			while (new_capacity < cap)
				new_capacity *= 2;

			T* new_buffer = (T*)::operator new (sizeof(T) * new_capacity);

			if constexpr (std::is_trivially_copyable<T>())
			{
				u8* src = (u8*)m_buffer;
				u8* len = src + m_count * sizeof(T);
				u8* dst = (u8*)new_buffer;
				while (src < len) *(dst++) = *(src++);
			}
			else
			{
				for (int i = 0; i < m_count; i++)
				{
					if (i < new_capacity)
						new (new_buffer + i) T(std::move(m_buffer[i]));
					m_buffer[i].~T();
				}
			}

			::operator delete (m_buffer, sizeof(T) * m_capacity);

			m_buffer = new_buffer;
			m_capacity = new_capacity;
		}
	}

	template<class T>
	inline void Vector<T>::resize(int new_count)
	{
		if (new_count < m_count)
			erase(new_count, m_count - new_count);
		else
			expand(new_count - m_count);
	}

	template<class T>
	inline T* Vector<T>::expand(int amount)
	{
		if (amount > 0)
		{
			int count = m_count;

			reserve(count + amount);
			for (int i = 0; i < amount; i++)
				new (m_buffer + count + i) T();

			m_count += amount;
			return &m_buffer[count];
		}

		return m_buffer;
	}

	template<class T>
	inline void Vector<T>::push_back(const T& item)
	{
		reserve(m_count + 1);
		new (m_buffer + m_count) T(item);
		m_count++;
	}

	template<class T>
	inline void Vector<T>::push_back(T&& item)
	{
		reserve(m_count + 1);
		new (m_buffer + m_count) T(std::move(item));
		m_count++;
	}

	template<class T>
	template<class ...Args>
	inline void Vector<T>::emplace_back(Args&& ...args)
	{
		reserve(m_count + 1);
		new (m_buffer + m_count) T(std::forward<Args>(args)...);
		m_count++;
	}

	template<class T>
	inline T& Vector<T>::operator[](int index)
	{
		BLAH_ASSERT(index >= 0 && index < m_count, "Index out of range");
		return m_buffer[index];
	}

	template<class T>
	inline const T& Vector<T>::operator[](int index) const
	{
		BLAH_ASSERT(index >= 0 && index < m_count, "Index out of range");
		return m_buffer[index];
	}

	template<class T>
	inline T* Vector<T>::data()
	{
		return m_buffer;
	}

	template<class T>
	inline const T* Vector<T>::data() const
	{
		return m_buffer;
	}

	template<class T>
	inline T* Vector<T>::begin()
	{
		return m_buffer;
	}

	template<class T>
	inline const T* Vector<T>::begin() const
	{
		return m_buffer;
	}

	template<class T>
	inline T* Vector<T>::end()
	{
		return m_buffer + m_count;
	}

	template<class T>
	inline const T* Vector<T>::end() const
	{
		return m_buffer + m_count;
	}

	template<class T>
	inline T& Vector<T>::front()
	{
		BLAH_ASSERT(m_count > 0, "Index out of range");
		return m_buffer[0];
	}

	template<class T>
	inline const T& Vector<T>::front() const
	{
		BLAH_ASSERT(m_count > 0, "Index out of range");
		return m_buffer[0];
	}

	template<class T>
	inline T& Vector<T>::back()
	{
		BLAH_ASSERT(m_count > 0, "Index out of range");
		return m_buffer[m_count - 1];
	}

	template<class T>
	inline const T& Vector<T>::back() const
	{
		BLAH_ASSERT(m_count > 0, "Index out of range");
		return m_buffer[m_count - 1];
	}

	template<class T>
	inline void Vector<T>::erase(int index, int elements)
	{
		BLAH_ASSERT(index >= 0 && index + elements <= m_count, "Index out of range");

		if (elements >= 1)
		{
			for (int i = index; i < (m_count - elements); i++)
				m_buffer[i] = std::move(m_buffer[i + elements]);
			for (int i = m_count - elements; i < m_count; i++)
				m_buffer[i].~T();
			m_count -= elements;
		}
	}

	template<class T>
	inline T Vector<T>::pop()
	{
		BLAH_ASSERT(m_count > 0, "Index out of range");

		T value = std::move(m_buffer[m_count - 1]);
		m_buffer[m_count - 1].~T();
		m_count--;
		return value;
	}
}