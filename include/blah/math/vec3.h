#pragma once
#include "calc.h"

namespace Blah
{
	struct Vec3
	{
		float x;
		float y;
		float z;

		Vec3()
			: x(0)
			, y(0)
			, z(0)
		{}

		Vec3(float x, float y, float z)
			: x(x)
			, y(y)
			, z(z)
		{}

		inline Vec3 operator +(const Vec3 rhs) const
		{
			return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
		}

		inline Vec3 operator -(const Vec3 rhs) const
		{
			return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
		}

		inline Vec3 normal() const
		{
			float ls = x * x + y * y + z * z;
			float length = (float)Calc::sqrt(ls);
			return Vec3(x / length, y / length, z / length);
		}

		static inline float dot(Vec3 a, Vec3 b)
		{
			return a.x * b.x +
				a.y * b.y +
				a.z * b.z;
		}

		static inline Vec3 cross(Vec3 a, Vec3 b)
		{
			return Vec3(
				a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x);
		}
	};
}