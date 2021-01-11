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
		
		Vec2 normal() const;
		Vec2 turn_right() const;
		Vec2 turn_left() const;
		float length() const;
		float length_squared() const;
		Vec2 perpendicular() const;
		float angle() const;
		
		static float dot(Vec2 a, Vec2 b);
		static float dot(float x, float y, Vec2 b);
		static float dot(float x1, float y1, float x2, float y2);

		static Vec2 transform(const Vec2& vec, const Mat3x2& matrix);
		static Vec2 transform(float x, float y, const Mat3x2& matrix);
		static Vec2 from_angle(float radians, float length);
		static Vec2 from_angle(float radians);
		static Vec2 lerp(Vec2 start, Vec2 end, float t);
		static Vec2 bezier_lerp(Vec2 start, Vec2 b, Vec2 end, float t);
		static Vec2 bezier_lerp(Vec2 start, Vec2 b, Vec2 c, Vec2 end, float t);
		static Vec2 reflect(const Vec2& vector, const Vec2& normal);

		static const Vec2 unit_x;
		static const Vec2 unit_y;
		static const Vec2 right;
		static const Vec2 up;
		static const Vec2 down;
		static const Vec2 left;
		static const Vec2 zero;
		static const Vec2 one;
		static const Vec2 up_right;
		static const Vec2 up_left;
		static const Vec2 down_right;
		static const Vec2 down_left;
	};
	
}