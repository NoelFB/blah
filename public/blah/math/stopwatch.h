#pragma once
#include <inttypes.h>

namespace Blah
{
	class Stopwatch
	{
	public:
		Stopwatch();
		void reset();
		uint64_t microseconds();
		uint64_t milliseconds();
	private:
		uint64_t start_time;
	};
}