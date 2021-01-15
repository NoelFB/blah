#include <blah/math/vec2.h>
#include <blah/math/mat3x2.h>
#include <blah/math/calc.h>
#include <math.h>

using namespace Blah;

Vec2::Vec2() { x = y = 0; }
Vec2::Vec2(float vx, float vy) { x = vx; y = vy; }
Vec2::Vec2(int vx, float vy) { x = (float)vx; y = vy; }
Vec2::Vec2(float vx, int vy) { x = vx; y = (float)vy; }
Vec2::Vec2(int vx, int vy) { x = (float)vx; y = (float)vy; }
Vec2::Vec2(Point p) { x = (float)p.x; y = (float)p.y; }

Vec2 Vec2::operator +(const Vec2 rhs) const { return Vec2(x + rhs.x, y + rhs.y); }
Vec2 Vec2::operator -(const Vec2 rhs) const { return Vec2(x - rhs.x, y - rhs.y); }
Vec2 Vec2::operator /(const float rhs) const { return Vec2(x / rhs, y / rhs); }
Vec2 Vec2::operator *(const float rhs) const { return Vec2(x * rhs, y * rhs); }
Vec2 Vec2::operator-() const { return Vec2(-x, -y); }

Vec2& Vec2::operator +=(const Vec2& rhs) { x += rhs.x; y += rhs.y; return *this; }
Vec2& Vec2::operator -=(const Vec2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
Vec2& Vec2::operator /=(const Vec2& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
Vec2& Vec2::operator *=(const Vec2& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
Vec2& Vec2::operator/=(float rhs) { x /= rhs; y /= rhs; return *this; }
Vec2& Vec2::operator*=(float rhs) { x *= rhs; y *= rhs; return *this; }

bool Vec2::operator ==(const Vec2& rhs) const { return x == rhs.x && y == rhs.y; }
bool Vec2::operator !=(const Vec2& rhs) const { return x != rhs.x || y != rhs.y; }

Vec2 Vec2::normal() const
{
	if (x == 0 && y == 0)
		return zero;
	float length = this->length();
	return Vec2(x / length, y / length);
}

Vec2 Vec2::turn_right() const
{
	return Vec2(y, -x);
}

Vec2 Vec2::turn_left() const
{
	return Vec2(-y, x);
}

float Vec2::length() const { return sqrtf(x * x + y * y); }
float Vec2::length_squared() const { return x * x + y * y; }

Vec2 Vec2::perpendicular() const
{
	return Vec2(-y, x);
}

float Vec2::angle() const
{
	return Calc::atan2(y, x);
}

float Vec2::dot(Vec2 a, Vec2 b) { return (a.x * b.x + a.y * b.y); }
float Vec2::dot(float x, float y, Vec2 b) { return (x * b.x + y * b.y); }
float Vec2::dot(float x1, float y1, float x2, float y2) { return (x1 * x2 + y1 * y2); }

Vec2 Vec2::transform(const Vec2& vec, const Mat3x2& matrix)
{
	return Vec2(
		(vec.x * matrix.m11) + (vec.y * matrix.m21) + matrix.m31,
		(vec.x * matrix.m12) + (vec.y * matrix.m22) + matrix.m32);
}

Vec2 Vec2::transform(float x, float y, const Mat3x2& matrix)
{
	return Vec2(
		(x * matrix.m11) + (y * matrix.m21) + matrix.m31,
		(x * matrix.m12) + (y * matrix.m22) + matrix.m32);
}

Vec2 Vec2::from_angle(float radians, float length)
{
	return Vec2((float)cos(radians) * length, (float)sin(radians) * length);
}

Vec2 Vec2::from_angle(float radians)
{
	return from_angle(radians, 1);
}

Vec2 Vec2::lerp(Vec2 a, Vec2 b, float t)
{
	if (t == 0)
		return a;
	else if (t == 1)
		return b;
	else
		return a + (b - a) * t;
}

Vec2 Vec2::bezier_lerp(Vec2 start, Vec2 b, Vec2 end, float t)
{
	return lerp(lerp(start, b, t), lerp(b, end, t), t);
}

Vec2 Vec2::bezier_lerp(Vec2 start, Vec2 b, Vec2 c, Vec2 end, float t)
{
	return bezier_lerp(lerp(start, b, t), lerp(b, c, t), lerp(c, end, t), t);
}

Vec2 Vec2::reflect(const Vec2& vector, const Vec2& normal)
{
	float dot = vector.x * normal.x + vector.y * normal.y;

	return Vec2(
		vector.x - 2.0f * dot * normal.x,
		vector.y - 2.0f * dot * normal.y);
}

const Vec2 Vec2::unit_x = Vec2(1, 0);
const Vec2 Vec2::unit_y = Vec2(0, 1);
const Vec2 Vec2::right = Vec2(1, 0);
const Vec2 Vec2::up = Vec2(0, -1);
const Vec2 Vec2::down = Vec2(0, 1);
const Vec2 Vec2::left = Vec2(-1, 0);
const Vec2 Vec2::zero = Vec2(0, 0);
const Vec2 Vec2::one = Vec2(1, 1);

#define DIAGONAL_UNIT 0.70710678118f
const Vec2 Vec2::up_right = Vec2(DIAGONAL_UNIT, -DIAGONAL_UNIT);
const Vec2 Vec2::up_left = Vec2(-DIAGONAL_UNIT, -DIAGONAL_UNIT);
const Vec2 Vec2::down_right = Vec2(DIAGONAL_UNIT, DIAGONAL_UNIT);
const Vec2 Vec2::down_left = Vec2(-DIAGONAL_UNIT, DIAGONAL_UNIT);
#undef DIAGONAL_UNIT