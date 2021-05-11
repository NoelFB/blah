#pragma once
#include <blah/numerics/vec2.h>

namespace Blah
{
	struct Circle
	{
		Vec2 center;
		float radius;

		constexpr Circle()
			: center(), radius(0) {}

		constexpr Circle(Vec2 center, float radius)
			: center(center), radius(radius) {}

		constexpr Circle(float x, float y, float radius)
			: center(x, y), radius(radius) {}

		constexpr void project(const Vec2& axis, float* min, float* max) const
		{
			*min = Vec2::dot(center - axis * radius, axis);
			*max = Vec2::dot(center + axis * radius, axis);
		}
	};
}