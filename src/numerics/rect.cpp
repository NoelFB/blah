#include <blah/numerics/rect.h>
#include <blah/numerics/point.h>
#include <blah/numerics/rectI.h>
#include <blah/numerics/vec2.h>
#include <blah/numerics/calc.h>
#include <blah/numerics/mat3x2.h>

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

float Rect::left() const
{
	return x;
}

float Rect::right() const
{
	return x + w;
}

float Rect::top() const
{
	return y;
}

float Rect::bottom() const
{
	return y + h;
}

Vec2 Rect::center() const
{
	return Vec2(x + w / 2, y + h / 2);
}

float Rect::center_x() const
{
	return x + w / 2;
}

float Rect::center_y() const
{
	return y + h / 2;
}

Vec2 Rect::top_left() const
{
	return Vec2(x, y);
}

Vec2 Rect::top_right() const
{
	return Vec2(x + w, y);
}

Vec2 Rect::bottom_right() const
{
	return Vec2(x + w, y + h);
}

Vec2 Rect::bottom_left() const
{
	return Vec2(x, y + h);
}

Vec2 Rect::center_left() const
{
	return Vec2(x, y + h / 2);
}

Vec2 Rect::center_right() const
{
	return Vec2(x + w, y + h / 2);
}

Vec2 Rect::middle_top() const
{
	return Vec2(x + w / 2, y);
}

Vec2 Rect::middle_bottom() const
{
	return Vec2(x + w / 2, y + h);
}

Line Rect::left_line() const
{
	return Line(left(), top(), left(), bottom());
}
Line Rect::right_line() const
{
	return Line(right(), top(), right(), bottom());
}
Line Rect::top_line() const
{
	return Line(left(), top(), right(), top());
}
Line Rect::bottom_line() const
{
	return Line(left(), bottom(), right(), bottom());
}

bool Rect::contains(const Point& pt) const
{
	return pt.x >= x && pt.x < x + w && pt.y >= y && pt.y < y + h;
}

bool Rect::contains(const Vec2& pt) const
{
	return pt.x >= x && pt.x < x + w && pt.y >= y && pt.y < y + h;
}

bool Rect::overlaps(const Rect& rect) const
{
	return x + w >= rect.x && y + h >= rect.y && x < rect.x + rect.w && y < rect.y + rect.h;
}

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

Rect Rect::inflate(float amount) const
{
	return Rect(x - amount, y - amount, w + amount * 2, h + amount * 2);
}

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