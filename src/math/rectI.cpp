#include <blah/math/rectI.h>
#include <blah/math/rect.h>
#include <blah/math/point.h>
#include <blah/math/vec2.h>
#include <blah/math/calc.h>

using namespace Blah;

RectI::RectI()
{
	x = y = w = h = 0;
}

RectI::RectI(int rx, int ry, int rw, int rh)
{
	x = rx;
	y = ry;
	w = rw;
	h = rh;
}

RectI::RectI(Point pos, Point size)
{
	x = pos.x;
	y = pos.y;
	w = size.x;
	h = size.y;
}

int RectI::left() const
{
	return x;
}

int RectI::right() const
{
	return x + w;
}

int RectI::top() const
{
	return y;
}

int RectI::bottom() const
{
	return y + h;
}

int RectI::center_x() const
{
	return x + w / 2;
}

int RectI::center_y() const
{
	return y + h / 2;
}

Point RectI::center() const
{
	return Point(x + w / 2, y + h / 2);
}

Point RectI::top_left() const
{
	return Point(x, y);
}

Point RectI::top_right() const
{
	return Point(x + w, y);
}

Point RectI::bottom_left() const
{
	return Point(x, y + h);
}

Point RectI::bottom_right() const
{
	return Point(x + w, y + h);
}

bool RectI::overlaps(const RectI& other) const
{
	return x < other.x + other.w
		&& other.x < x + w
		&& y < other.y + other.h
		&& other.y < y + h;
}

RectI RectI::overlap_rect(const Rect& against) const
{
	RectI result = *this;

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

bool RectI::contains(const Point& point) const
{
	return point.x >= x && point.x < x + w && point.y >= y && point.y < y + h;
}

bool RectI::contains(const Vec2& point) const
{
	return point.x >= x && point.x < x + w && point.y >= y && point.y < y + h;
}

char RectI::get_sector(const Point& pt) const
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

char RectI::get_sector(const Vec2& pt) const
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

bool RectI::operator==(const RectI& rhs) const
{
	return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h;
}

bool RectI::operator!=(const RectI& rhs) const
{
	return !(*this == rhs);
}

RectI RectI::operator+(const Point& rhs) const
{
	return RectI(x + rhs.x, y + rhs.y, w, h);
}

RectI RectI::operator-(const Point& rhs) const
{
	return RectI(x - rhs.x, y - rhs.y, w, h);
}

RectI RectI::operator*(const int& rhs) const
{
	return RectI(x * rhs, y * rhs, w * rhs, h * rhs);
}

RectI RectI::operator/(const int& rhs) const
{
	return RectI(x / rhs, y / rhs, w / rhs, h / rhs);
}

RectI& RectI::operator+=(const Point& rhs)
{
	x += rhs.x; y += rhs.y; return *this;
}

RectI& RectI::operator-=(const Point& rhs)
{
	x -= rhs.x; y -= rhs.y; return *this;
}
