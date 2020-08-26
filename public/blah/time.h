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

		static void pause_for(float time);

		static bool on_interval(float time, float delta, float interval, float offset = 0);
		static bool on_interval(float delta, float interval, float offset = 0);
		static bool on_interval(float interval, float offset = 0);

		static bool on_time(float time, float timestamp);

		static bool between_interval(float time, float interval, float offset = 0);
		static bool between_interval(float interval, float offset = 0);
	};
}