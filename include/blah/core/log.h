#pragma once

// error / abort
#ifdef DEBUG

	#include <stdlib.h>
	#define BLAH_ERROR(message) \
		do { Blah::Log::error(message "\n\tin file: %s:%d", __FILE__, __LINE__); abort(); } while(0)

	#define BLAH_ERROR_FMT(message, ...) \
		do { Blah::Log::error(message "\n\tin file: %s:%d", __VA_ARGS__, __FILE__, __LINE__); abort(); } while(0)

#else

	#define BLAH_ERROR(message) \
		Blah::Log::error(message "\n\tin file: %s:%d", __FILE__, __LINE__)

	#define BLAH_ERROR_FMT(message, ...) \
		Blah::Log::error(message "\n\tin file: %s:%d", __VA_ARGS__, __FILE__, __LINE__)

#endif

#define BLAH_ASSERT(condition, message) \
		do { if (!(condition)) { BLAH_ERROR(message); } } while(0)

// maximum length of a print/warn/error message
#ifndef BLAH_MESSAGE
#define BLAH_MESSAGE 1024
#endif

namespace Blah
{
	namespace Log
	{
		void print(const char* info, ...);
		void warn(const char* info, ...);
		void error(const char* info, ...);
	}
}