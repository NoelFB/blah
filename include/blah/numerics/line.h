#pragma once
#include <blah/numerics/vec2.h>

namespace Blah
{
	struct Rect;

	struct Line
	{
		Vec2 a;
		Vec2 b;

		constexpr Line()
			: a(), b() {}

		constexpr Line(const Vec2& a, const Vec2& b)
			: a(a), b(b) {}

		constexpr Line(float x0, float y0, float x1, float y1)
			: a(x0, y0), b(x1, y1) {}

		Rect bounds() const;

		Vec2 closest_point(const Vec2& pt) const;
		bool intersects(const Rect& rect) const;
		bool intersects(const Rect& rect, Vec2* out_intersection_point) const;
		bool intersects(const Line& line) const;
		bool intersects(const Line& line, Vec2* out_intersection_point) const;

		constexpr void project(const Vec2& axis, float* min, float* max) const
		{
			float dot = a.x * axis.x + a.y * axis.y;
			*min = dot;
			*max = dot;
			dot = b.x * axis.x + b.y * axis.y;
			*min = dot < *min ? dot : *min;
			*max = dot > *max ? dot : *max;
		}

		constexpr Line operator +(const Vec2& rhs) const
		{
			return Line(a + rhs, b + rhs);
		}

		constexpr Line operator -(const Vec2& rhs) const
		{
			return Line(a - rhs, b - rhs);
		}
	};
}