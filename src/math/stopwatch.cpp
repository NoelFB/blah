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

uint64_t Stopwatch::milliseconds()
{
	return microseconds() / 1000;
}


uint64_t Stopwatch::microseconds()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(system_clock::now().time_since_epoch()).count() - start_time;
}
