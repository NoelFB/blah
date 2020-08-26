#include <blah/math/vec2.h>
#include <blah/math/mat3x2.h>
#include <math.h>

using namespace Blah;

Point::Point() 
{ 
	x = y = 0; 
}

Point::Point(int px, int py) 
{ 
	x = px; 
	y = py; 
}

Point Point::operator +(const Point rhs) const { return Point(x + rhs.x, y + rhs.y); }
Point Point::operator -(const Point rhs) const { return Point(x - rhs.x, y - rhs.y); }
Point Point::operator /(const int rhs) const { return Point(x / rhs, y / rhs); }
Point Point::operator *(const int rhs) const { return Point(x * rhs, y * rhs); }
Point Point::operator-() const { return Point(-x, -y); }

Point& Point::operator +=(const Point& rhs) { x += rhs.x; y += rhs.y; return *this; }
Point& Point::operator -=(const Point& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
Point& Point::operator /=(const Point& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
Point& Point::operator *=(const Point& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
Point& Point::operator /=(int rhs) { x /= rhs; y /= rhs; return *this; }
Point& Point::operator *=(int rhs) { x *= rhs; y *= rhs; return *this; }

bool Point::operator ==(const Point& rhs) { return x == rhs.x && y == rhs.y; }
bool Point::operator !=(const Point& rhs) { return x != rhs.x || y != rhs.y; }

float Point::length() const 
{ 
	return sqrtf((float)(x * x + y * y));
}

int Point::length_squared() const 
{ 
	return x * x + y * y; 
}

const Point Point::unitX = Point(1, 0);
const Point Point::unitY = Point(0, 1);
const Point Point::right = Point(1, 0);
const Point Point::up = Point(0, -1);
const Point Point::down = Point(0, 1);
const Point Point::left = Point(-1, 0);
const Point Point::zero = Point(0, 0);
const Point Point::one = Point(1, 1);
