#include <blah/math/stopwatch.h>
#include <chrono>

using namespace std::chrono;
using namespace Blah;

Stopwatch::Stopwatch()
{
	reset();
}

void Stopwatch::reset()
{
	start_time = std::chrono::duration_cast<std::chrono::microseconds>(system_clock::now().time_since_epoch()).count();
}

u64 Stopwatch::milliseconds()
{
	return microseconds() / 1000;
}


u64 Stopwatch::microseconds()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(system_clock::now().time_since_epoch()).count() - start_time;
}
