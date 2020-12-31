#pragma once
#include <blah/math/vec2.h>

namespace Blah
{
	struct Rect;

	struct Line
	{
		Vec2 a;
		Vec2 b;

		Line() {}
		Line(float x0, float y0, float x1, float y1);
		Line(const Vec2& start, const Vec2& end);

		Rect bounds() const;

		Vec2 closest_point(const Vec2& pt) const;
		bool intersects(const Rect& rect) const;
		bool intersects(const Rect& rect, Vec2* out_intersection_point) const;
		bool intersects(const Line& line) const;
		bool intersects(const Line& line, Vec2* out_intersection_point) const;

		void project(const Vec2& axis, float* min, float* max) const;

		Line operator +(const Vec2& rhs) const;
		Line operator -(const Vec2& rhs) const;
	};
}