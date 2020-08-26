#include <blah/math/rect.h>
#include <blah/math/point.h>
#include <blah/math/rectI.h>
#include <blah/math/vec2.h>
#include <blah/math/calc.h>
#include <blah/math/mat3x2.h>

using namespace Blah;

Rect::Rect() 
{ 
	x = y = w = h = 0; 
};

Rect::Rect(float rx, float ry, float rw, float rh) 
{ 
	x = rx; 
	y = ry; 
	w = rw; 
	h = rh; 
}

Rect::Rect(Vec2 pos, Vec2 size)
{
	x = pos.x;
	y = pos.y;
	w = size.x;
	h = size.y;
}

Rect::Rect(RectI r) 
{ 
	x = (float)r.x; 
	y = (float)r.y;
	w = (float)r.w;
	h = (float)r.h;
}

Rect Rect::scale(float s)
{ 
	x = (x * s); 
	y = (y * s); 
	w = (w * s); 
	h = (h * s); 
	return *this; 
}

Rect Rect::scale(float sx, float sy) 
{ 
	x = (x * sx); 
	y = (y * sy); 
	w = (w * sx); 
	h = (h * sy); 
	return *this; 
}

Line Rect::left_line() const { return Line(left(), top(), left(), bottom()); }
Line Rect::right_line() const { return Line(right(), top(), right(), bottom()); }
Line Rect::top_line() const { return Line(left(), top(), right(), top()); }
Line Rect::bottom_line() const { return Line(left(), bottom(), right(), bottom()); }

Rect Rect::operator+(const Vec2& rhs) const { return Rect(x + rhs.x, y + rhs.y, w, h); }
Rect Rect::operator-(const Vec2& rhs) const { return Rect(x - rhs.x, y - rhs.y, w, h); }
Rect& Rect::operator+=(const Vec2& rhs) { x += rhs.x; y += rhs.y; return *this; }
Rect& Rect::operator-=(const Vec2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }

Rect Rect::transform(const Rect& rect, const Mat3x2& matrix)
{
	return Rect(
		(rect.x * matrix.m11) + (rect.y * matrix.m21) + matrix.m31, 
		(rect.x * matrix.m12) + (rect.y * matrix.m22) + matrix.m32,
		(rect.w * matrix.m11) + (rect.h * matrix.m21),
		(rect.w * matrix.m12) + (rect.h * matrix.m22));
}

Rect Rect::transform(float x, float y, float w, float h, const Mat3x2& matrix)
{
	return Rect(
		(x * matrix.m11) + (y * matrix.m21) + matrix.m31,
		(x * matrix.m12) + (y * matrix.m22) + matrix.m32,
		(w * matrix.m11) + (h * matrix.m21),
		(w * matrix.m12) + (h * matrix.m22));
}

Rect Rect::from_points(Vec2& from, Vec2& to)
{
	Vec2 min = Vec2(Calc::min(from.x, to.x), Calc::min(from.y, to.y));
	Vec2 max = Vec2(Calc::max(from.x, to.x), Calc::max(from.y, to.y));
	return Rect(min.x, min.y, max.x - min.x, max.y - min.y);
}

char Rect::get_sector(const Vec2& pt) const
{
	char h;
	if (pt.x < left())
		h = 0b0001;
	else if (pt.x >= right())
		h = 0b0010;
	else
		h = 0;

	char v;
	if (pt.y < top())
		v = 0b0100;
	else if (pt.y >= bottom())
		v = 0b1000;
	else
		v = 0;

	return h | v;
}