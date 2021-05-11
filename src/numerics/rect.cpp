#include <blah/numerics/rect.h>
#include <blah/numerics/calc.h>

using namespace Blah;

Rect Rect::overlap_rect(const Rect& against) const
{
	Rect result(0, 0, 0, 0);

	if (x + w >= against.x && x < against.x + against.w)
	{
		result.x = Calc::max(x, against.x);
		result.w = Calc::min(x + w, against.x + against.w) - result.x;
	}

	if (y + h >= against.y && y < against.y + against.h)
	{
		result.y = Calc::max(y, against.y);
		result.h = Calc::min(y + h, against.y + against.h) - result.y;
	}

	return result;
}

bool Rect::intersects(const Line& line) const
{
	return line.intersects(*this);
}

bool Rect::intersects(const Line& line, Vec2* out_intersection_point) const
{
	return line.intersects(*this, out_intersection_point);
}

bool Rect::intersects(const Vec2& line_from, const Vec2& line_to) const
{
	return intersects(Line(line_from, line_to));
}

bool Rect::intersects(const Vec2& line_from, const Vec2& line_to, Vec2* out_intersection_point) const
{
	return intersects(Line(line_from, line_to), out_intersection_point);
}

Vec2 Rect::intersection_point(const Line& line) const
{
	Vec2 ret;
	if (line.intersects(*this, &ret))
		return ret;
	else
		return Vec2::zero;
}

Vec2 Rect::intersection_point(const Vec2& line_from, const Vec2& line_to) const
{
	Vec2 ret;
	if (Line(line_from, line_to).intersects(*this, &ret))
		return ret;
	else
		return Vec2::zero;
}