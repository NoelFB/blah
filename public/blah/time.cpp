#include <blah/time.h>
#include <math.h>

using namespace Blah;

namespace
{
	float modf(float x, float m) 
	{ 
		return x - (int)(x / m) * m; 
	}
}

uint64_t Time::milliseconds = 0;
float Time::elapsed = 0;
float Time::delta = 0;
float Time::previous_elapsed = 0;
float Time::pause_timer = 0;

void Time::pause_for(float time)
{
	if (time >= pause_timer)
		pause_timer = time;
}

bool Time::on_interval(float time, float delta, float interval, float offset)
{
	return floor((time - offset - delta) / interval) < floor((time - offset) / interval);
}

bool Time::on_interval(float delta, float interval, float offset)
{
	return Time::on_interval(Time::elapsed, delta, interval, offset);
}

bool Time::on_interval(float interval, float offset)
{
	return Time::on_interval(Time::elapsed, Time::delta, interval, offset);
}

bool Time::on_time(float time, float timestamp)
{
	return time >= timestamp && time - Time::delta < timestamp;
}

bool Time::between_interval(float time, float interval, float offset)
{
	return modf(time - offset, interval * 2) >= interval;
}

bool Time::between_interval(float interval, float offset)
{
	return between_interval(Time::elapsed, interval, offset);
}
