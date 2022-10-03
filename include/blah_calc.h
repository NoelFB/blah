#pragma once
#include <blah_common.h>

namespace Blah
{
	enum class Endian
	{
		Little,
		Big
	};

	namespace Calc
	{
		constexpr f32 PI = 3.141592653f;
		constexpr f32 TAU = PI * 2.0f;
		constexpr f32 RIGHT = 0;
		constexpr f32 LEFT = PI;
		constexpr f32 UP = PI / -2;
		constexpr f32 DOWN = PI / 2;

		constexpr f32 mod(f32 x, f32 m) { return x - (int)(x / m) * m; }
		inline f32 sin(f32 x) { return ::sinf(x); }
		inline f32 cos(f32 x) { return ::cosf(x); }
		inline f32 tan(f32 x) { return ::tanf(x); }
		inline f32 atan2(f32 y, f32 x) { return ::atan2f(y, x); }
		inline f32 round(f32 x) { return ::roundf(x); }
		inline f32 floor(f32 x) { return ::floorf(x); }
		inline f32 ceiling(f32 x) { return ::ceilf(x); }
		inline f32 pow(f32 x, f32 n) { return ::powf(x, n); }
		inline f32 sqrt(f32 x) { return ::sqrtf(x); }

		template<class T>
		constexpr T sign(T x)
		{
			return static_cast<T>(x == 0 ? 0 : (x < 0 ? -1 : 1));
		}

		template<class T>
		constexpr T abs(T x)
		{
			return x < 0 ? -x : x;
		}

		template<class T, class TMin, class TMax>
		constexpr T clamp(T value, TMin min, TMax max)
		{
			return value < min ? static_cast<T>(min) : (value > max ? static_cast<T>(max) : value);
		}

		template<class T>
		constexpr T min(T a, T b)
		{
			return  (T)(a < b ? a : b);
		}

		template<class T, typename ... Args>
		constexpr T min(const T& a, const T& b, const Args&... args)
		{
			return Calc::min(a, Calc::min(b, args...));
		}

		template<class T>
		constexpr T max(T a, T b)
		{
			return (T)(a > b ? a : b);
		}

		template<class T, typename ... Args>
		constexpr T max(const T& a, const T& b, const Args&... args)
		{
			return Calc::max(a, Calc::max(b, args...));
		}
		
		constexpr f32 approach(f32 t, f32 target, f32 delta)
		{
			return t < target ? Calc::min(t + delta, target) : Calc::max(t - delta, target);
		}

		constexpr f32 map(f32 t, f32 old_min, f32 old_max, f32 new_min, f32 new_max)
		{
			return new_min + ((t - old_min) / (old_max - old_min)) * (new_max - new_min);
		}

		constexpr f32 clamped_map(f32 t, f32 old_min, f32 old_max, f32 new_min, f32 new_max)
		{
			return map(Calc::clamp(t, old_min, old_max), old_min, old_max, new_min, new_max);
		}

		constexpr f32 lerp(f32 a, f32 b, f32 t)
		{
			return a + (b - a) * t;
		}

		constexpr f32 angle_diff(f32 radians_a, f32 radians_b)
		{
			return Calc::mod((radians_b - radians_a) + PI, TAU) - PI;
		}

		constexpr f32 angle_lerp(f32 radians_a, f32 radians_b, f32 p)
		{
			const auto shortest_angle = Calc::mod(Calc::mod(radians_b - radians_a, TAU) + (TAU + PI), TAU) - PI;
			return radians_a + Calc::mod(shortest_angle * p, TAU);
		}

		template<class T>
		constexpr void swap_endian(T* value)
		{
			for (int i = 0; i < sizeof(T) / 2; i++)
			{
				char* _ptr = (char*)&value;
				char _temp = *(_ptr + i);
				*(_ptr + i) = *(_ptr + sizeof(T) - i - 1);
				*(_ptr + sizeof(T) - i - 1) = _temp;
			}
		}

		inline bool is_big_endian()
		{
			return (*((short*)"AB") == 0x4243);	
		}

		inline bool is_little_endian()
		{
			return (*((short*)"AB") != 0x4243);
		}

		inline bool is_endian(const Endian& endian)
		{
			return
				(endian == Endian::Little && is_little_endian()) ||
				(endian == Endian::Big && is_big_endian());
		}
	};
}
