#include <blah/math/mat3x2.h>
#include <blah/math/vec2.h>
#include <string.h>
#include <math.h>

using namespace Blah;

Mat3x2::Mat3x2()
	: m11(0), m12(0), m21(0), m22(0), m31(0), m32(0) {}

Mat3x2::Mat3x2(float m11, float m12, float m21, float m22, float m31, float m32)
	: m11(m11), m12(m12), m21(m21), m22(m22), m31(m31), m32(m32) {}

Mat3x2 Mat3x2::invert() const
{
	auto det = (m11 * m22) - (m21 * m12);
	auto invDet = 1.0f / det;

	return Mat3x2
	{
		m22 * invDet,
		-m12 * invDet,
		-m21 * invDet,
		m11 * invDet,
		(m21 * m32 - m31 * m22) * invDet,
		(m31 * m12 - m11 * m32) * invDet
	};
}

float Mat3x2::scaling_factor() const
{
	return (float)sqrt((double)m11 * m11 + (double)m12 * m12);
}

const Mat3x2 Mat3x2::identity = Mat3x2(1, 0, 0, 1, 0, 0);

Mat3x2 Mat3x2::operator *(const Mat3x2& rhs) const { return multiply(*this, rhs); }
Mat3x2 Mat3x2::operator +(const Mat3x2& rhs) const { return add(*this, rhs); }
Mat3x2 Mat3x2::operator -(const Mat3x2& rhs) const { return subtract(*this, rhs); }

Mat3x2& Mat3x2::operator*=(const Mat3x2& rhs)
{
	*this = multiply(*this, rhs);
	return *this;
}

bool Mat3x2::operator ==(const Mat3x2& rhs)
{
	return memcmp(this, &rhs, sizeof(Mat3x2));
}

bool Mat3x2::operator !=(const Mat3x2& rhs)
{
	return !(*this == rhs);
}

Mat3x2 Mat3x2::create_translation(const Vec2& Vec2)
{
	return create_translation(Vec2.x, Vec2.y);
}

Mat3x2 Mat3x2::create_translation(float x, float y)
{
	return Mat3x2(1, 0, 0, 1, x, y);
}

Mat3x2 Mat3x2::create_scale(float scale)
{
	return create_scale(scale, scale);
}

Mat3x2 Mat3x2::create_scale(Vec2 Vec2)
{
	return create_scale(Vec2.x, Vec2.y);
}

Mat3x2 Mat3x2::create_scale(float x, float y)
{
	return Mat3x2(x, 0, 0, y, 0, 0);
}

Mat3x2 Mat3x2::create_scale(float scale, Vec2 centerPoint)
{
	Mat3x2 result;

	float tx = centerPoint.x * (1 - scale);
	float ty = centerPoint.y * (1 - scale);

	result.m11 = scale;
	result.m12 = 0.0f;
	result.m21 = 0.0f;
	result.m22 = scale;
	result.m31 = tx;
	result.m32 = ty;

	return result;
}

Mat3x2 Mat3x2::create_scale(Vec2 scale, Vec2 centerPoint)
{
	Mat3x2 result;

	float tx = centerPoint.x * (1 - scale.x);
	float ty = centerPoint.y * (1 - scale.y);

	result.m11 = scale.x;
	result.m12 = 0.0f;
	result.m21 = 0.0f;
	result.m22 = scale.y;
	result.m31 = tx;
	result.m32 = ty;

	return result;
}

Mat3x2 Mat3x2::create_scale(float scaleX, float scaleY, Vec2 centerPoint)
{
	Mat3x2 result;

	float tx = centerPoint.x * (1 - scaleX);
	float ty = centerPoint.y * (1 - scaleY);

	result.m11 = scaleX;
	result.m12 = 0.0f;
	result.m21 = 0.0f;
	result.m22 = scaleY;
	result.m31 = tx;
	result.m32 = ty;

	return result;
}

Mat3x2 Mat3x2::create_rotation(float radians)
{
	float c = cosf(radians);
	float s = sinf(radians);

	return Mat3x2(c, s, -s, c, 0, 0);
}

Mat3x2 Mat3x2::create_transform(const Vec2& position, const Vec2& origin, const Vec2& scale, float rotation)
{
	Mat3x2 matrix = identity;

	if (origin.x != 0 || origin.y != 0)
		matrix = create_translation(-origin.x, -origin.y);
	if (scale.x != 1 || scale.y != 1)
		matrix = matrix * create_scale(scale);
	if (rotation != 0)
		matrix = matrix * create_rotation(rotation);
	if (position.x != 0 || position.y != 0)
		matrix = matrix * create_translation(position);
	
	return matrix;
}

Mat3x2 Mat3x2::add(const Mat3x2& a, const Mat3x2& b)
{
	return Mat3x2(
		a.m11 + b.m11,
		a.m12 + b.m12,
		a.m21 + b.m21,
		a.m22 + b.m22,
		a.m31 + b.m31,
		a.m32 + b.m32);
}

Mat3x2 Mat3x2::subtract(const Mat3x2& a, const Mat3x2& b)
{
	return Mat3x2(
		a.m11 - b.m11,
		a.m12 - b.m12,
		a.m21 - b.m21,
		a.m22 - b.m22,
		a.m31 - b.m31,
		a.m32 - b.m32);
}

Mat3x2 Mat3x2::multiply(const Mat3x2& a, const Mat3x2& b)
{
	return Mat3x2(a.m11 * b.m11 + a.m12 * b.m21,
		a.m11 * b.m12 + a.m12 * b.m22,
		a.m21 * b.m11 + a.m22 * b.m21,
		a.m21 * b.m12 + a.m22 * b.m22,
		a.m31 * b.m11 + a.m32 * b.m21 + b.m31,
		a.m31 * b.m12 + a.m32 * b.m22 + b.m32);
}