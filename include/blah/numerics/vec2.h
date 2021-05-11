#pragma once
#include <blah/numerics/point.h>
#include <blah/numerics/calc.h>
#include <blah/numerics/mat3x2.h>

namespace Blah
{
	struct Vec2
	{
		float x;
		float y;

		constexpr Vec2()
			: x(0), y(0) {}

		constexpr Vec2(float x, float y)
			: x(x), y(y) {}

		constexpr Vec2(int x, float y)
			: x(static_cast<float>(x)), y(y) {}

		constexpr Vec2(float x, int y)
			: x(x), y(static_cast<float>(y)) {}

		constexpr Vec2(int x, int y)
			: x(static_cast<float>(x)), y(static_cast<float>(y)) {}

		constexpr Vec2(const Point& p)
			: x(static_cast<float>(p.x)), y(static_cast<float>(p.y)) {}
		
		constexpr Vec2 operator +(const Vec2 rhs) const
		{
			return Vec2(x + rhs.x, y + rhs.y);
		}

		constexpr Vec2 operator -(const Vec2 rhs) const
		{
			return Vec2(x - rhs.x, y - rhs.y);
		}

		constexpr Vec2 operator /(const float rhs) const
		{
			return Vec2(x / rhs, y / rhs);
		}

		constexpr Vec2 operator *(const float rhs) const
		{
			return Vec2(x * rhs, y * rhs);
		}

		constexpr Vec2 operator -() const
		{
			return Vec2(-x, -y);
		}

		constexpr Vec2& operator +=(const Vec2& rhs)
		{
			x += rhs.x; y += rhs.y;
			return *this;
		}

		constexpr Vec2& operator -=(const Vec2& rhs)
		{
			x -= rhs.x; y -= rhs.y;
			return *this;
		}

		constexpr Vec2& operator /=(const Vec2& rhs)
		{
			x /= rhs.x; y /= rhs.y;
			return *this;
		}

		constexpr Vec2& operator *=(const Vec2& rhs)
		{
			x *= rhs.x; y *= rhs.y;
			return *this;
		}

		constexpr Vec2& operator /=(float rhs)
		{
			x /= rhs; y /= rhs;
			return *this;
		}

		constexpr Vec2& operator *=(float rhs)
		{
			x *= rhs; y *= rhs;
			return *this;
		}

		constexpr bool operator ==(const Vec2& rhs) const
		{
			return x == rhs.x && y == rhs.y;
		}

		constexpr bool operator !=(const Vec2& rhs) const
		{
			return x != rhs.x || y != rhs.y;
		}

		// Returns the absolute value of the Vector
		constexpr Vec2 abs() const
		{
			return Vec2(x < 0 ? -x : x, y < 0 ? -y : 0);
		}

		// Returns the Normalized Vector
		// If the length is 0, the resulting Vector is 0,0
		Vec2 normal() const
		{
			if (x == 0 && y == 0)
				return Vec2(0, 0);

			const float len = Calc::sqrt((x * x) + (y * y));
			return Vec2(x / len, y / len);
		}

		// Rotates the Vector 90 degrees right (-y, x)
		constexpr Vec2 turn_right() const
		{
			return Vec2(-y, x);
		}

		// Rotates the Vector 90 degrees left (y, -x)
		constexpr Vec2 turn_left() const
		{
			return Vec2(y, -x);
		}

		// Returns the length of the Vector
		float length() const
		{
			return Calc::sqrt((x * x) + (y * y));
		}

		// Returns the squared length of the Vector
		constexpr float length_squared() const
		{
			return (x * x) + (y * y);
		}

		// Gets the perpendicular Vector (-y, x)
		constexpr Vec2 perpendicular() const
		{
			return Vec2(-y, x);
		}

		// Gets the angle, in radians, of the Vector
		float angle() const
		{
			return Calc::atan2(y, x);
		}
		
		// Calculates the Dot Product between two vectors
		static constexpr float dot(Vec2 a, Vec2 b)
		{
			return (a.x * b.x + a.y * b.y);
		}

		// Calculates the Dot Product between two vectors
		static constexpr float dot(float x, float y, Vec2 b)
		{
			return (x * b.x + y * b.y);
		}

		// Calculates the Dot Product between two vectors
		static constexpr float dot(float x1, float y1, float x2, float y2)
		{
			return (x1 * x2 + y1 * y2);
		}

		// Transforms a Vector by the given Matrix
		static constexpr Vec2 transform(const Vec2& vec, const Mat3x2& matrix)
		{
			return Vec2(
				(vec.x * matrix.m11) + (vec.y * matrix.m21) + matrix.m31,
				(vec.x * matrix.m12) + (vec.y * matrix.m22) + matrix.m32);
		}

		// Transforms a Vector by the given Matrix
		static constexpr Vec2 transform(float x, float y, const Mat3x2& matrix)
		{
			return Vec2(
				(x * matrix.m11) + (y * matrix.m21) + matrix.m31,
				(x * matrix.m12) + (y * matrix.m22) + matrix.m32);
		}

		// Transforms a Vector Normal by the given Matrix
		static constexpr Vec2 transform_normal(const Vec2& vec, const Mat3x2& matrix)
		{
			return Vec2(
				vec.x * matrix.m11 + vec.y * matrix.m21,
				vec.x * matrix.m12 + vec.y * matrix.m22);
		}

		// Transforms a Vector Normal by the given Matrix
		static constexpr Vec2 transform_normal(float x, float y, const Mat3x2& matrix)
		{
			return Vec2(
				x * matrix.m11 + y * matrix.m21,
				x * matrix.m12 + y * matrix.m22);
		}

		// Calculates a Vector value from the given radians
		static Vec2 from_angle(float radians, float length = 1.0f)
		{
			return Vec2(
				Calc::cos(radians) * length, 
				Calc::sin(radians) * length);
		}

		// Lerps between two Vectors
		static constexpr Vec2 lerp(Vec2 a, Vec2 b, float t)
		{
			if (t == 0)
				return a;
			else if (t == 1)
				return b;
			else
				return a + (b - a) * t;
		}

		// Lerps between two Vectors along a Bezier curve
		static constexpr Vec2 lerp_bezier(Vec2 a, Vec2 b, Vec2 end, float t)
		{
			return lerp(lerp(a, b, t), lerp(b, end, t), t);
		}

		// Lerps between two Vectors along a Bezier curve
		static constexpr Vec2 lerp_bezier(Vec2 a, Vec2 b, Vec2 c, Vec2 end, float t)
		{
			return lerp_bezier(lerp(a, b, t), lerp(b, c, t), lerp(c, end, t), t);
		}

		// Reflects a vector along the given Normal
		static constexpr Vec2 reflect(const Vec2& vector, const Vec2& normal)
		{
			const float dot = vector.x * normal.x + vector.y * normal.y;

			return Vec2(
				vector.x - 2.0f * dot * normal.x,
				vector.y - 2.0f * dot * normal.y);
		}

		// Gets the minimum between two vectors
		static constexpr Vec2 min(const Vec2& a, const Vec2& b)
		{
			return Vec2(
				a.x < b.x ? a.x : b.x,
				a.y < b.y ? a.y : b.y);
		}

		// Gets the maximum between two vectors
		static constexpr Vec2 max(const Vec2& a, const Vec2& b)
		{
			return Vec2(
				a.x > b.x ? a.x : b.x,
				a.y > b.y ? a.y : b.y);
		}

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

	inline const Vec2 Vec2::unit_x     = Vec2(1, 0);
	inline const Vec2 Vec2::unit_y     = Vec2(0, 1);
	inline const Vec2 Vec2::right      = Vec2(1, 0);
	inline const Vec2 Vec2::up         = Vec2(0, -1);
	inline const Vec2 Vec2::down       = Vec2(0, 1);
	inline const Vec2 Vec2::left       = Vec2(-1, 0);
	inline const Vec2 Vec2::zero       = Vec2(0, 0);
	inline const Vec2 Vec2::one        = Vec2(1, 1);
	inline const Vec2 Vec2::up_right   = Vec2(0.70710678118f, -0.70710678118f);
	inline const Vec2 Vec2::up_left    = Vec2(-0.70710678118f, -0.70710678118f);
	inline const Vec2 Vec2::down_right = Vec2(0.70710678118f, 0.70710678118f);
	inline const Vec2 Vec2::down_left  = Vec2(-0.70710678118f, 0.70710678118f);
	
}