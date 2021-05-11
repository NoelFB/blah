#pragma once
#include <blah/numerics/vec2.h>

namespace Blah
{
	struct Quad
	{
		Vec2 a;
		Vec2 b;
		Vec2 c;
		Vec2 d;

		constexpr Quad()
			: a(), b(), c(), d() {}

		constexpr Quad(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
			: a(a), b(b), c(c), d(d) {}

		constexpr void project(const Vec2& axis, float* min, float* max) const
		{
			float dot = Vec2::dot(a, axis);
			*min = dot;
			*max = dot;
			dot = Vec2::dot(b, axis);
			*min = dot < *min ? dot : *min;
			*max = dot > *max ? dot : *max;
			dot = Vec2::dot(c, axis);
			*min = dot < *min ? dot : *min;
			*max = dot > *max ? dot : *max;
			dot = Vec2::dot(d, axis);
			*min = dot < *min ? dot : *min;
			*max = dot > *max ? dot : *max;
		}
	};
}