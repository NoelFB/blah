#include <blah/math/calc.h>
#include <math.h>
#include <stdlib.h>

using namespace Blah;

float Calc::rand_float(float min, float maxExc)
{
	return min + rand_float(maxExc - min);
}

float Calc::rand_float(float maxExc)
{
	return (rand() / (float)RAND_MAX) * maxExc;
}

int Calc::rand_int(int min, int maxExc)
{
	return min + rand_int(maxExc - min);
}

int Calc::rand_int(int maxExc)
{
	if (maxExc <= 0)
		return 0;
	return rand() % maxExc;
}

int Calc::rand_int()
{
	return rand();
}

float Calc::approach(float t, float target, float delta)
{
	return t < target ? min(t + delta, target) : max(t - delta, target);
}

Vec2 Calc::approach(const Vec2& t, const Vec2& target, float delta)
{
	if ((target - t).length() <= delta)
		return target;
	return t + (target - t).normal() * delta;
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

bool Calc::is_big_endian()
{
	return (*((short*)"AB") == 0x4243);
}

bool Calc::is_little_endian()
{
	return (*((short*)"AB") != 0x4243);
}