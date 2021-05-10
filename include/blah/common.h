#pragma once
#include <cstdint>     // for integer types
#include <cstddef>     // for size_t type
#include <memory>      // for std::shared_ptr
#include <functional>  // for std::function

// Asserts
#if defined(DEBUG) || defined(_DEBUG)
#include <cstdlib>     // for abort
#define BLAH_ASSERT(condition, msg) \
		do { if (!(condition)) { Blah::Log::error("%s\n\tin %s:%d", (msg), __FILE__, __LINE__); abort(); } } while(0)
#else
#define BLAH_ASSERT(condition, msg) \
		do { if (!(condition)) { Blah::Log::error("%s\n\tin %s:%d", (msg), __FILE__, __LINE__); } } while(0)
#endif

// maximum length of a print/warn/error message
#ifndef BLAH_MESSAGE
#define BLAH_MESSAGE 1024
#endif

namespace Blah
{
	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using WeakRef = std::weak_ptr<T>;

	template<typename T>
	using Func = std::function<T>;

	namespace Log
	{
		enum class Category
		{
			Info,
			Warning,
			Error
		};

		void info(const char* message, ...);
		void warn(const char* message, ...);
		void error(const char* message, ...);
	}
}