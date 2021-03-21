#pragma once
#include <blah/core/common.h>

namespace Blah
{
	class Stopwatch
	{
	public:
		Stopwatch();
		void reset();
		u64 microseconds();
		u64 milliseconds();
	private:
		u64 start_time;
	};
}