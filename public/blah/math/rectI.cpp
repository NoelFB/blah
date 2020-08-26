#include <blah/math/rectI.h>
#include <blah/math/rect.h>
#include <blah/math/point.h>
#include <blah/math/vec2.h>

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

Point RectI::center() const { return Point(centerX(), centerY()); }
Point RectI::top_left() const { return Point(left(), top()); }
Point RectI::top_right() const { return Point(right(), top()); }
Point RectI::bottom_left() const { return Point(left(), bottom()); }
Point RectI::bottom_right() const { return Point(right(), bottom()); }

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

RectI RectI::operator+(const Point& rhs) const { return RectI(x + rhs.x, y + rhs.y, w, h); }
RectI RectI::operator-(const Point& rhs) const { return RectI(x - rhs.x, y - rhs.y, w, h); }
RectI& RectI::operator+=(const Point& rhs) { x += rhs.x; y += rhs.y; return *this; }
RectI& RectI::operator-=(const Point& rhs) { x -= rhs.x; y -= rhs.y; return *this; }