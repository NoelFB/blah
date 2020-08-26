#pragma once

// Error / Abort
#ifdef DEBUG

	#define BLAH_ERROR(message) \
		do { Log::error(message "\n\tin file: %s:%d", __FILE__, __LINE__); abort(); } while(0)

	#define BLAH_ASSERT(condition, message) \
		do { if (!(condition)) { BLAH_ERROR(message); } } while(0)

#else

	#define BLAH_ERROR(message) \
		Log::error(message "\n\tin file: %s:%d", __FILE__, __LINE__)

	#define BLAH_ASSERT(condition, message) \
		do { } while(false)

#endif

#ifndef BLAH_MAX_MESSAGE
#define BLAH_MAX_MESSAGE 1024
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