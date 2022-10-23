#pragma once
#include <new>         // for in-place constructors, for Vector/StackVector
#include <utility>     // for std::move, std::forward
#include <stdint.h>    // for integer types
#include <stddef.h>    // for std::size_t
#include <stdlib.h>    // for abort
#include <stdarg.h>    // for string format methods
#include <stdio.h>     // for vsnprintf
#include <math.h>      // for standard lib math functions used in blah_calc.h

// Numeric Types
namespace Blah
{
	using i8       = int8_t;
	using i16      = int16_t;
	using i32      = int32_t;
	using i64      = int64_t;
	using u8       = uint8_t;
	using u16      = uint16_t;
	using u32      = uint32_t;
	using u64      = uint64_t;
	using f32      = float;
	using f64      = double;
	using size_t   = std::size_t;
}

// Aborts
#ifdef _WIN32
#define BLAH_ABORT() do { __debugbreak(); ::exit(1); } while(0)
#else
#define BLAH_ABORT() ::abort()
#endif

// Assert Behavior (abort in debug)
#if defined(DEBUG) || defined(_DEBUG)
#define BLAH_ASSERT_BEHAVIOR() BLAH_ABORT()
#else
#define BLAH_ASSERT_BEHAVIOR()
#endif

// Asserts
#define BLAH_ASSERT(condition, msg) \
	do { \
		if (!(condition)) { \
			Blah::Log::error("%s\n\tin %s:%d", (msg), __FILE__, __LINE__); \
			BLAH_ASSERT_BEHAVIOR(); \
		} \
	} while(0)
#define BLAH_ASSERT_FMT(condition, msg, ...) \
	do { \
		if (!(condition)) { \
			Blah::Log::error(msg "\n\tin %s:%d", __VA_ARGS__, __FILE__, __LINE__); \
			BLAH_ASSERT_BEHAVIOR(); \
		} \
	} while(0)

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
	struct RefDel { virtual ~RefDel() = default; virtual void delete_it(void* it) = 0; };
	template<class Y> struct RefDelOf : public RefDel { void delete_it(void* it) { delete (Y*)it; } };

	template<typename T>
	class Ref
	{
	template<class Y> friend class Ref;
	private:
		inline static RefDelOf<T> del;

		T* m_obj = nullptr;
		u32* m_num = nullptr;
		RefDel* m_del = nullptr;

		Ref(T* o, u32* n, RefDel* d) : m_obj(o), m_num(n), m_del(d) {}

		void copy(const Ref& rhs) { m_obj = rhs.m_obj; m_num = rhs.m_num; m_del = rhs.m_del; }
		void zero() { m_obj = nullptr; m_num = nullptr; m_del = nullptr; }
		void steal(Ref& rhs) { copy(rhs); rhs.zero(); }
		void increment() { if (m_num) (*m_num)++; }

	public:
		Ref() = default;
		template<class Y>
		Ref(Y* instance) : Ref(static_cast<T*>(instance), new u32(1), &Ref<Y>::del) {}
		Ref(const Ref& rhs) { copy(rhs); increment(); }
		Ref(Ref&& rhs) { steal(rhs); }
		~Ref() { reset(); }

		Ref& operator=(const Ref& rhs) { if (this != &rhs) { reset(); copy(rhs); increment(); } return *this; }
		Ref& operator=(Ref&& rhs) { if (this != &rhs) { reset(); steal(rhs); } return *this; }

		void reset() { if (m_num && --(*m_num) <= 0) { m_del->delete_it(m_obj); delete m_num; } zero(); }
		int use_count() const { return (m_num ? (*m_num) : 0); }
		T* get() const { return m_obj; }
		T* operator->() const { return m_obj; }
		operator bool() const { return m_num && (*m_num) > 0; }
		template<class Y> bool operator==(const Ref<Y>& rhs) const { return m_num == rhs.m_num; }
		template<class Y> bool operator!=(const Ref<Y>& rhs) const { return m_num != rhs.m_num; }
		template<class Y> operator Ref<Y>() { increment(); return Ref<Y>(static_cast<Y*>(m_obj), m_num, m_del); }
	};
}
#endif
