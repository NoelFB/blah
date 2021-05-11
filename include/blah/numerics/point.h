#pragma once
#include <blah/numerics/calc.h>

namespace Blah
{
	struct Point
	{
		int x;
		int y;

		constexpr Point()
			: x(0), y(0) {}

		constexpr Point(int x, int y)
			: x(x), y(y) {}

		constexpr Point operator +(const Point rhs) const
		{
			return Point(x + rhs.x, y + rhs.y);
		}

		constexpr Point operator -(const Point rhs) const
		{
			return Point(x - rhs.x, y - rhs.y);
		}

		constexpr Point operator /(const int rhs) const
		{
			return Point(x / rhs, y / rhs);
		}

		constexpr Point operator *(const int rhs) const
		{
			return Point(x * rhs, y * rhs);
		}

		constexpr Point operator -() const
		{
			return Point(-x, -y);
		}


		constexpr Point& operator +=(const Point& rhs)
		{
			x += rhs.x; y += rhs.y; 
			return *this;
		}

		constexpr Point& operator -=(const Point& rhs)
		{
			x -= rhs.x; y -= rhs.y;
			return *this;
		}

		constexpr Point& operator /=(const Point& rhs)
		{
			x /= rhs.x; y /= rhs.y;
			return *this;
		}

		constexpr Point& operator *=(const Point& rhs)
		{
			x *= rhs.x; y *= rhs.y;
			return *this;
		}

		constexpr Point& operator /=(int rhs)
		{
			x /= rhs; y /= rhs;
			return *this;
		}

		constexpr Point& operator *=(int rhs)
		{
			x *= rhs; y *= rhs;
			return *this;
		}

		constexpr bool operator ==(const Point& rhs) const
		{
			return x == rhs.x && y == rhs.y;
		}

		constexpr bool operator !=(const Point& rhs) const
		{
			return x != rhs.x || y != rhs.y;
		}

		float length() const
		{
			return Calc::sqrt((float)(x * x + y * y));
		}

		constexpr int length_squared() const
		{
			return x * x + y * y;
		}

		static const Point unitX;
		static const Point unitY;
		static const Point right;
		static const Point up;
		static const Point down;
		static const Point left;
		static const Point zero;
		static const Point one;
	};

	inline const Point Point::unitX = Point(1, 0);
	inline const Point Point::unitY = Point(0, 1);
	inline const Point Point::right = Point(1, 0);
	inline const Point Point::up    = Point(0, -1);
	inline const Point Point::down  = Point(0, 1);
	inline const Point Point::left  = Point(-1, 0);
	inline const Point Point::zero  = Point(0, 0);
	inline const Point Point::one   = Point(1, 1);
}