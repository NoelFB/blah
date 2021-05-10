#include <blah/numerics/line.h>
#include <blah/numerics/rect.h>
#include <blah/numerics/vec2.h>
#include <blah/numerics/calc.h>

using namespace Blah;

Line::Line(float x0, float y0, float x1, float y1)
{
	a.x = x0;
	a.y = y0;
	b.x = x1;
	b.y = y1;
}

Line::Line(const Vec2& start, const Vec2& end)
	: a(start), b(end) {}

Rect Line::bounds() const
{
	Vec2 pos = Vec2(Calc::min(a.x, b.x), Calc::min(a.y, b.y));

	return Rect(
		pos,
		Vec2(Calc::max(a.x, b.x) - pos.x, Calc::max(a.y, b.y) - pos.y)
	);
}

Vec2 Line::closest_point(const Vec2& pt) const
{
	Vec2 v = b - a;
	Vec2 w = pt - a;
	float t = Vec2::dot(w, v) / Vec2::dot(v, v);
	t = Calc::clamp(t, 0.0f, 1.0f);

	return v * t + a;
}

bool Line::intersects(const Rect& rect) const
{
	char ca = rect.get_sector(a);
	char cb = rect.get_sector(b);

	if (ca == cb || (ca & cb) != 0)
		return false;

	char both = ca | cb;

	// top
	if ((both & 0b0100) != 0 && intersects(rect.top_line()))
		return true;

	// bottom
	if ((both & 0b1000) != 0 && intersects(rect.bottom_line()))
		return true;

	// left
	if ((both & 0b0001) != 0 && intersects(rect.left_line()))
		return true;

	// right
	if ((both & 0b0010) != 0 && intersects(rect.right_line()))
		return true;

	return false;
}

bool Line::intersects(const Rect& rect, Vec2* out_intersection_point) const
{
	char ca = rect.get_sector(a);
	char cb = rect.get_sector(b);

	if (ca == cb || (ca & cb) != 0)
		return false;

	char both = ca | cb;

	// top
	if ((both & 0b0100) != 0 && intersects(rect.top_line(), out_intersection_point))
		return true;

	// bottom
	if ((both & 0b1000) != 0 && intersects(rect.bottom_line(), out_intersection_point))
		return true;

	// left
	if ((both & 0b0001) != 0 && intersects(rect.left_line(), out_intersection_point))
		return true;

	// right
	if ((both & 0b0010) != 0 && intersects(rect.right_line(), out_intersection_point))
		return true;

	return false;
}

bool Line::intersects(const Line& line) const
{
	Vec2 e = b - a;
	Vec2 d = line.b - line.a;
	float e_dot_d_perp = e.x * d.y - e.y * d.x;

	// if e dot d == 0, it means the lines are parallel
	// so have infinite intersection points
	if (e_dot_d_perp < 0.0001 && e_dot_d_perp > -0.0001)
		return false;

	Vec2 c = line.a - a;
	float t = (c.x * d.y - c.y * d.x) / e_dot_d_perp;
	if (t < 0 || t > 1)
		return false;

	float u = (c.x * e.y - c.y * e.x) / e_dot_d_perp;
	if (u < 0 || u > 1)
		return false;

	return true;
}

bool Line::intersects(const Line& line, Vec2* intersection_point) const
{
	Vec2 e = b - a;
	Vec2 d = line.b - line.a;
	float e_dot_d_perp = e.x * d.y - e.y * d.x;

	// if e dot d == 0, it means the lines are parallel
	// so have infinite intersection points
	if (e_dot_d_perp < 0.0001 && e_dot_d_perp > -0.0001)
		return false;

	Vec2 c = line.a - a;
	float t = (c.x * d.y - c.y * d.x) / e_dot_d_perp;
	if (t < 0 || t > 1)
		return false;

	float u = (c.x * e.y - c.y * e.x) / e_dot_d_perp;
	if (u < 0 || u > 1)
		return false;

	Vec2 i = (e * t) + a;
	intersection_point->x = i.x;
	intersection_point->y = i.y;
	return true;
}

void Line::project(const Vec2& axis, float* min, float* max) const
{
	float dot = a.x * axis.x + a.y * axis.y;
	*min = dot;
	*max = dot;
	dot = b.x * axis.x + b.y * axis.y;
	*min = Calc::min(dot, *min);
	*max = Calc::max(dot, *max);
}

Line Line::operator +(const Vec2& rhs) const { return Line(a + rhs, b + rhs); }
Line Line::operator -(const Vec2& rhs) const { return Line(a - rhs, b - rhs); }
