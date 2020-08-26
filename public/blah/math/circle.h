#pragma once
#include <blah/math/vec2.h>

namespace Blah
{
	struct Circle
	{
		Vec2 center;
		float radius;

		Circle()
			: center(), radius(0) {}

		Circle(Vec2 center, float radius)
			: center(center), radius(radius) {}

		void project(const Vec2& axis, float* min, float* max) const;
	};
}