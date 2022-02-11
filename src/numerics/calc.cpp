#include <blah/numerics/calc.h>
#include <blah/numerics/spatial.h>
#include <cmath>
#include <cstdlib>

using namespace Blah;

float Calc::approach(float t, float target, float delta)
{
	return t < target ? min(t + delta, target) : max(t - delta, target);
}

float Calc::map(float t, float old_min, float old_max, float new_min, float new_max)
{
	return new_min + ((t - old_min) / (old_max - old_min)) * (new_max - new_min);
}

float Calc::clamped_map(float t, float old_min, float old_max, float new_min, float new_max)
{
	return map(Calc::clamp(t, old_min, old_max), old_min, old_max, new_min, new_max);
}

int Calc::sign(int x)
{
	return (x < 0 ? -1 : (x > 0 ? 1 : 0));
}

float Calc::sign(float x)
{
	return (x < 0 ? -1.0f : (x > 0 ? 1.0f : 0.0f));
}

int Calc::abs(int x)
{
	return x < 0 ? -x : x;
}

float Calc::abs(float x)
{
	return x < 0 ? -x : x;
}

float Calc::round(float x)
{
	return roundf(x);
}

float Calc::floor(float x)
{
	return floorf(x);
}

float Calc::ceiling(float x)
{
	return ceilf(x);
}

float Calc::mod(float x, float m)
{
	return x - (int)(x / m) * m;
}

float Calc::sin(float x)
{
	return sinf(x);
}

float Calc::cos(float x)
{
	return cosf(x);
}

float Calc::tan(float x)
{
	return tanf(x);
}

float Calc::atan2(float y, float x)
{
	return atan2f(y, x);
}

float Calc::pow(float x, float n)
{
	return powf(x, n);
}

float Calc::sqrt(float x)
{
	return sqrtf(x);
}

float Calc::snap(float val, float interval)
{
	if (val > 0)
		return ((int)((val + interval / 2) / interval)) * interval;
	else
		return ((int)((val - interval / 2) / interval)) * interval;
}

float Calc::angle_diff(float radians_a, float radians_b)
{
	return mod((radians_b - radians_a) + PI, TAU) - PI;
}

float Calc::angle_lerp(float radians_a, float radians_b, float p)
{
	const auto shortest_angle = mod(mod(radians_b - radians_a, TAU) + (TAU + PI), TAU) - PI;
	return radians_a + mod(shortest_angle * p, TAU);
}

float Calc::lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}