#pragma once
#include <blah_common.h>

namespace Blah
{
	// A mix between an std::vector and an std::array
	// The goal is to have an std::vector implementation that lives
	// on the stack instead of the heap
	template<class T, size_t Capacity>
	class StackVector
	{
	private:
		u8 m_buffer[sizeof(T) * Capacity];
		int m_count;

	public:
		static constexpr size_t capacity = Capacity;

		StackVector();
		StackVector(const InitializerList<T>& list);
		StackVector(const StackVector& src);
		StackVector(StackVector&& src) noexcept;
		~StackVector();

		StackVector& operator=(const StackVector& src);
		StackVector& operator=(StackVector&& src) noexcept;

		void clear();

		int size() const;

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

	template<class T, size_t Capacity>
	inline StackVector<T, Capacity>::StackVector()
	{
		m_count = 0;
	}

	template<class T, size_t Capacity>
	inline StackVector<T, Capacity>::StackVector(const InitializerList<T>& init)
	{
		m_count = 0;
		for (auto& it : init)
			push_back(it);
	}

	template<class T, size_t Capacity>
	inline StackVector<T, Capacity>::StackVector(const StackVector& src)
	{
		for (int i = 0; i < src.m_count; i++)
			new (data() + i) T(src.data()[i]);
		m_count = src.m_count;
	}

	template<class T, size_t Capacity>
	inline StackVector<T, Capacity>::StackVector(StackVector&& src) noexcept
	{
		for (int i = 0; i < src.m_count; i++)
			new (data() + i) T(std::move(src.data()[i]));
		m_count = src.m_count;
	}

	template<class T, size_t Capacity>
	inline StackVector<T, Capacity>::~StackVector()
	{
		clear();
	}

	template<class T, size_t Capacity>
	inline StackVector<T, Capacity>& StackVector<T, Capacity>::operator=(const StackVector& src)
	{
		clear();

		for (int i = 0; i < src.m_count; i++)
			new (data() + i) T(std::move(src.data()[i]));
		m_count = src.m_count;

		return *this;
	}

	template<class T, size_t Capacity>
	inline StackVector<T, Capacity>& StackVector<T, Capacity>::operator=(StackVector&& src) noexcept
	{
		clear();

		for (int i = 0; i < src.m_count; i++)
			new (data() + i) T(std::move(src.data()[i]));
		m_count = src.m_count;

		return *this;
	}

	template<class T, size_t Capacity>
	inline void StackVector<T, Capacity>::clear()
	{
		for (int i = 0; i < m_count; i++)
			data()[i].~T();
		m_count = 0;
	}

	template<class T, size_t Capacity>
	inline int StackVector<T, Capacity>::size() const
	{
		return m_count;
	}

	template<class T, size_t Capacity>
	inline T* StackVector<T, Capacity>::expand(int amount)
	{
		BLAH_ASSERT(m_count + amount <= Capacity, "Exceeded StackVector Capacity");

		if (amount > 0)
		{
			int count = m_count;

			for (int i = 0; i < amount; i++)
				new (data() + count + i) T();

			m_count += amount;
			return &data()[count];
		}

		return (T*)m_buffer;
	}

	template<class T, size_t Capacity>
	inline void StackVector<T, Capacity>::push_back(const T& item)
	{
		BLAH_ASSERT(m_count + 1 <= Capacity, "Exceeded StackVector Capacity");
		new (data() + m_count) T(item);
		m_count++;
	}

	template<class T, size_t Capacity>
	inline void StackVector<T, Capacity>::push_back(T&& item)
	{
		BLAH_ASSERT(m_count + 1 <= Capacity, "Exceeded StackVector Capacity");
		new (data() + m_count) T(std::move(item));
		m_count++;
	}

	template<class T, size_t Capacity>
	template<class ...Args>
	inline void StackVector<T, Capacity>::emplace_back(Args && ...args)
	{
		BLAH_ASSERT(m_count + 1 <= Capacity, "Exceeded StackVector Capacity");
		new (data() + m_count) T(std::forward<Args>(args)...);
		m_count++;
	}

	template<class T, size_t Capacity>
	inline T& StackVector<T, Capacity>::operator[](int index)
	{
		BLAH_ASSERT(index >= 0 && index < m_count, "Index out of range");
		return data()[index];
	}

	template<class T, size_t Capacity>
	inline const T& StackVector<T, Capacity>::operator[](int index) const
	{
		BLAH_ASSERT(index >= 0 && index < m_count, "Index out of range");
		return data()[index];
	}

	template<class T, size_t Capacity>
	inline T* StackVector<T, Capacity>::data()
	{
		return (T*)m_buffer;
	}

	template<class T, size_t Capacity>
	inline const T* StackVector<T, Capacity>::data() const
	{
		return (T*)m_buffer;
	}

	template<class T, size_t Capacity>
	inline T* StackVector<T, Capacity>::begin()
	{
		return (T*)m_buffer;
	}

	template<class T, size_t Capacity>
	inline const T* StackVector<T, Capacity>::begin() const
	{
		return (T*)m_buffer;
	}

	template<class T, size_t Capacity>
	inline T* StackVector<T, Capacity>::end()
	{
		return ((T*)m_buffer) + m_count;
	}

	template<class T, size_t Capacity>
	inline const T* StackVector<T, Capacity>::end() const
	{
		return ((T*)m_buffer) + m_count;
	}

	template<class T, size_t Capacity>
	inline T& StackVector<T, Capacity>::front()
	{
		BLAH_ASSERT(m_count > 0, "Index out of range");
		return data()[0];
	}

	template<class T, size_t Capacity>
	inline const T& StackVector<T, Capacity>::front() const
	{
		BLAH_ASSERT(m_count > 0, "Index out of range");
		return data()[0];
	}

	template<class T, size_t Capacity>
	inline T& StackVector<T, Capacity>::back()
	{
		BLAH_ASSERT(m_count > 0, "Index out of range");
		return data()[m_count - 1];
	}

	template<class T, size_t Capacity>
	inline const T& StackVector<T, Capacity>::back() const
	{
		BLAH_ASSERT(m_count > 0, "Index out of range");
		return data()[m_count - 1];
	}

	template<class T, size_t Capacity>
	inline void StackVector<T, Capacity>::erase(int index, int elements)
	{
		BLAH_ASSERT(index >= 0 && index + elements <= m_count, "Index out of range");

		if (elements >= 1)
		{
			for (int i = index; i < (m_count - elements); i++)
				data()[i] = std::move(data()[i + elements]);
			for (int i = m_count - elements; i < m_count; i++)
				data()[i].~T();
			m_count -= elements;
		}
	}

	template<class T, size_t Capacity>
	inline T StackVector<T, Capacity>::pop()
	{
		BLAH_ASSERT(m_count > 0, "Index out of range");

		T value = std::move(data()[m_count - 1]);
		data()[m_count - 1].~T();
		m_count--;
		return value;
	}
}
