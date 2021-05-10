#pragma once

namespace Blah
{
	struct Point
	{
		int x;
		int y;

		Point();
		Point(int px, int py);

		Point operator +(const Point rhs) const;
		Point operator -(const Point rhs) const;
		Point operator /(const int rhs) const;
		Point operator *(const int rhs) const;
		Point operator -() const;

		Point& operator +=(const Point& rhs);
		Point& operator -=(const Point& rhs);
		Point& operator /=(const Point& rhs);
		Point& operator *=(const Point& rhs);
		Point& operator /=(int rhs);
		Point& operator *=(int rhs);

		bool operator ==(const Point& rhs);
		bool operator !=(const Point& rhs);

		float length() const;
		int length_squared() const;

		static const Point unitX;
		static const Point unitY;
		static const Point right;
		static const Point up;
		static const Point down;
		static const Point left;
		static const Point zero;
		static const Point one;
	};
}