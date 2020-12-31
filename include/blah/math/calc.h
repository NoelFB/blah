#pragma once
#include <inttypes.h>

namespace Blah
{
	namespace Calc
	{
		constexpr float PI = 3.141592653f;
		constexpr float TAU = PI * 2.0f;
		constexpr float RIGHT = 0;
		constexpr float LEFT = PI;
		constexpr float UP = PI / -2;
		constexpr float DOWN = PI / 2;

		float rand_float(float min, float maxExc);
		
		float rand_float(float maxExc);

		int rand_int(int min, int maxExc);

		int rand_int(int maxExc);

		int rand_int();

		float approach(float t, float target, float maxDelta);

		float clamp(float t, float min, float max);

		int clamp_int(int t, int min, int max);

		float map(float t, float old_min, float old_max, float new_min, float new_max);

		float clamped_map(float t, float old_min, float old_max, float new_min, float new_max);

		int sign(int x);

		float sign(float x);

		int abs(int x);

		float abs(float x);

		template<class T, class U>
		T min(T a, U b) { return  (T)(a < b ? a : b); }

		template<class T, class U>
		T max(T a, U b) { return (T)(a > b ? a : b); }

		float round(float x);

		float floor(float x);

		float ceiling(float x);

		float mod(float x, float m);

		float sin(float x);

		float cos(float x);

		float atan2(float y, float x);

		float pow(float x, float n);

		float sqrt(float x);

		float snap(float val, float interval);

		float angle_diff(float radians_a, float radians_b);
		
		float angle_lerp(float radians_a, float radians_b, float p);

		float lerp(float a, float b, float t);

		bool is_big_endian();

		bool is_little_endian();
	};
}
