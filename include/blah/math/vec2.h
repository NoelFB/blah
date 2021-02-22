#pragma once
#include <blah/math/point.h>

namespace Blah
{

	struct Mat3x2;

	struct Vec2
	{
		float x;
		float y;

		Vec2();
		Vec2(float vx, float vy);
		Vec2(int vx, float vy);
		Vec2(float vx, int vy);
		Vec2(int vx, int vy);
		Vec2(Point p);
		
		Vec2 operator +(const Vec2 rhs) const;
		Vec2 operator -(const Vec2 rhs) const;
		Vec2 operator /(const float rhs) const;
		Vec2 operator *(const float rhs) const;
		Vec2 operator -() const;
		
		Vec2& operator +=(const Vec2& rhs);
		Vec2& operator -=(const Vec2& rhs);
		Vec2& operator /=(const Vec2& rhs);
		Vec2& operator *=(const Vec2& rhs);
		Vec2& operator /=(float rhs);
		Vec2& operator *=(float rhs);
		
		bool operator ==(const Vec2& rhs) const;
		bool operator !=(const Vec2& rhs) const;

		// Returns the absolute value of the Vector
		Vec2 abs() const;

		// Returns the Normalized Vector
		// If the length is 0, the resulting Vector is 0,0
		Vec2 normal() const;

		// Rotates the Vector 90 degrees right (y, -x)
		Vec2 turn_right() const;

		// Rotates the Vector 90 degrees left (-y, x)
		Vec2 turn_left() const;

		// Returns the length of the Vector
		float length() const;

		// Returns the squared length of the Vector
		float length_squared() const;

		// Gets the perpendicular Vector (-y, x)
		Vec2 perpendicular() const;

		// Gets the angle, in radians, of the Vector
		float angle() const;
		
		// Calculates the Dot Product between two vectors
		static float dot(Vec2 a, Vec2 b);

		// Calculates the Dot Product between two vectors
		static float dot(float x, float y, Vec2 b);

		// Calculates the Dot Product between two vectors
		static float dot(float x1, float y1, float x2, float y2);

		// Transforms a Vector by the given Matrix
		static Vec2 transform(const Vec2& vec, const Mat3x2& matrix);

		// Transforms a Vector by the given Matrix
		static Vec2 transform(float x, float y, const Mat3x2& matrix);

		// Transforms a Vector Normal by the given Matrix
		static Vec2 transform_normal(const Vec2& vec, const Mat3x2& matrix);

		// Transforms a Vector Normal by the given Matrix
		static Vec2 transform_normal(float x, float y, const Mat3x2& matrix);

		// Calculates a Vector value from the given radians
		static Vec2 from_angle(float radians, float length = 1.0f);

		// Lerps between two Vectors
		static Vec2 lerp(Vec2 start, Vec2 end, float t);

		// Lerps between two Vectors along a Bezier curve
		static Vec2 lerp_bezier(Vec2 start, Vec2 b, Vec2 end, float t);

		// Lerps between two Vectors along a Bezier curve
		static Vec2 lerp_bezier(Vec2 start, Vec2 b, Vec2 c, Vec2 end, float t);

		// Reflects a vector along the given Normal
		static Vec2 reflect(const Vec2& vector, const Vec2& normal);

		// Gets the minimum between two vectors
		static Vec2 min(const Vec2& a, const Vec2& b);

		// Gets the maximum between two vectors
		static Vec2 max(const Vec2& a, const Vec2& b);

		// (1, 0)
		static const Vec2 unit_x;

		// (0, 1)
		static const Vec2 unit_y;

		// (1, 0)
		static const Vec2 right;

		// (0, -1)
		static const Vec2 up;

		// (0, 1)
		static const Vec2 down;

		// (-1, 0)
		static const Vec2 left;

		// (0, 0)
		static const Vec2 zero;

		// (1, 1)
		static const Vec2 one;

		// (0.707, -0.707)
		static const Vec2 up_right;

		// (-0.707, -0.707)
		static const Vec2 up_left;

		// (0.707, 0.707)
		static const Vec2 down_right;

		// (-0.707, 0.707)
		static const Vec2 down_left;
	};
	
}