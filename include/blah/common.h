#pragma once

#include <new>     // for in-place constructors, for Vector/StackVector
#include <utility> // for std::move

// Asserts
#if defined(DEBUG) || defined(_DEBUG)
#	include <stdlib.h>     // for abort
#	define BLAH_ASSERT(condition, msg) do { if (!(condition)) { Blah::Log::error("%s\n\tin %s:%d", (msg), __FILE__, __LINE__); abort(); } } while(0)
#else
#	define BLAH_ASSERT(condition, msg) do { if (!(condition)) { Blah::Log::error("%s\n\tin %s:%d", (msg), __FILE__, __LINE__); } } while(0)
#endif

// Numeric Types
#include <stdint.h>    // for integer types
#include <stddef.h>    // for size_t type
namespace Blah
{
	// Numeric Types
	using i8  = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;
	using u8  = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;
	using f32 = float;
	using f64 = double;
	using size_t = std::size_t;
}

// Logging
namespace Blah
{
	namespace Log
	{
		enum class Category
		{
			Info,
			Warning,
			Error
		};

		constexpr int max_length = 1024;

		void info(const char* message, ...);
		void warn(const char* message, ...);
		void error(const char* message, ...);
	}
}

// Initializer list, required for Vector/StackVector
#include <initializer_list>
namespace Blah
{
	template<typename T> using InitializerList = std::initializer_list<T>;
}

// Functional, for App Callbacks
#ifndef BLAH_NO_FUNCTIONAL
#include <functional>
namespace Blah
{
	template<class Ret, class...Args> using Func = std::function<Ret(Args...)>;
}
#else
namespace Blah
{
	template<class Ret, class...Args> using Func = Ret(*)(Args...);
}
#endif

// Ref Counter, for Graphics & Input Resources
#ifndef BLAH_NO_SHARED_PTR
#include <memory>
namespace Blah
{
	template<typename T> using Ref = std::shared_ptr<T>;
}
#else
namespace Blah
{
	template<typename T>
	class Ref
	{
	template<class Y> friend class Ref;
	private:
		T* m_instance;
		i32* m_counter;
		Ref(T* instance, i32* counter) : m_instance(instance), m_counter(counter) {}

	public:
		Ref() : m_instance(nullptr), m_counter(nullptr) {}

		template<class Y>
		Ref(Y* instance) : Ref(static_cast<T*>(instance), new i32(1)) {}
		Ref(const Ref& rhs) : Ref(rhs.m_instance, rhs.m_counter) { if (m_counter) (*m_counter)++; }
		Ref(Ref&& rhs) : Ref(rhs.m_instance, rhs.m_counter) { rhs.m_instance = nullptr; rhs.m_counter = nullptr; }

		Ref& operator=(const Ref& rhs)
		{
			if (this != &rhs)
			{
				reset();
				m_instance = rhs.m_instance; m_counter = rhs.m_counter;
				if (m_counter) (*m_counter)++;
			}
			return *this;
		}

		Ref& operator=(Ref&& rhs)
		{
			if (this != &rhs)
			{
				reset();
				m_instance = rhs.m_instance; m_counter = rhs.m_counter;
				rhs.m_instance = nullptr; rhs.m_counter = nullptr;
			}
			return *this;
		}

		~Ref() { reset(); }

		void reset()
		{
			if (m_counter) (*m_counter)--;
			if (m_counter && (*m_counter) <= 0) { delete m_instance; delete m_counter; }
			m_instance = nullptr; m_counter = nullptr;
		}

		int use_count() const { return (m_counter ? (*m_counter) : 0); }
		T* get() const { return m_instance; }
		T* operator->() const { return m_instance; }
		operator bool() const { return m_counter && (*m_counter) > 0; }
		template<class Y> bool operator==(const Ref<Y>& rhs) const { return m_counter == rhs.m_counter; }
		template<class Y> bool operator!=(const Ref<Y>& rhs) const { return m_counter != rhs.m_counter; }
		template<class Y> operator Ref<Y>() { if (m_counter) (*m_counter)++; return Ref<Y>(static_cast<Y*>(m_instance), m_counter); }
	};
}
#endif