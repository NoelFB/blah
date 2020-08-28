#pragma once
#include <inttypes.h>

namespace Blah
{
	struct Time
	{
		// uptime, in milliseconds
		static uint64_t milliseconds;

		// uptime, in seconds
		static float elapsed;

		// previous frame uptime, in seconds
		static float previous_elapsed;

		// delta time from last frame
		static float delta;

		// time the application should pause for
		static float pause_timer;

		// pauses the entire application for the given time
		static void pause_for(float time);

		// returns true on the given time interval
		static bool on_interval(float time, float delta, float interval, float offset = 0);

		// returns true on the given time interval
		static bool on_interval(float delta, float interval, float offset = 0);

		// returns true on the given time interval
		static bool on_interval(float interval, float offset = 0);

		// returns true when the given timestamp is passed
		static bool on_time(float time, float timestamp);

		// returns true between time intervals
		static bool between_interval(float time, float interval, float offset = 0);
		
		// returns true between time intervals
		static bool between_interval(float interval, float offset = 0);
	};
}