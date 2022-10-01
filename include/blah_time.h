#pragma once
#include <blah_common.h>

namespace Blah
{
	namespace Time
	{
		// ticks per second (microseconds, in this case)
		constexpr u64 ticks_per_second = 1000000;

		// uptime, in ticks, at the start of the current frame
		extern u64 ticks;

		// uptime, in seconds, at the start of the current frame
		extern double seconds;

		// delta time from last frame
		extern float delta;

		// previous frame uptime, in ticks
		extern u64 previous_ticks;

		// previous frame uptime, in seconds
		extern double previous_seconds;

		// time the application should pause for
		extern float pause_timer;

		// uptime, in ticks. polls the Platform for an immediate value, unlike the cached `Time::ticks` value
		u64 get_ticks();

		// pauses the entire application for the given time
		void pause_for(float duration);

		// returns true on the given time interval
		bool on_interval(double time, float delta, float interval, float offset);

		// returns true on the given time interval
		bool on_interval(float delta, float interval, float offset);

		// returns true on the given time interval
		bool on_interval(float interval, float offset = 0);

		// returns true when the given timestamp is passed
		bool on_time(double time, double timestamp);

		// returns true between time intervals
		bool between_interval(double time, float interval, float offset);
		
		// returns true between time intervals
		bool between_interval(float interval, float offset = 0);
	};

	class Stopwatch
	{
	public:
		Stopwatch();
		void reset();
		u64 microseconds() const;
		u64 milliseconds() const;

	private:
		u64 start_time;
	};
}