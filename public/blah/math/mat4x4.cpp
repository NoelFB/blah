#include <blah/math/mat4x4.h>

using namespace Blah;

Mat4x4::Mat4x4() :
	m11(0.0f), m12(0.0f), m13(0.0f), m14(0.0f),
	m21(0.0f), m22(0.0f), m23(0.0f), m24(0.0f),
	m31(0.0f), m32(0.0f), m33(0.0f), m34(0.0f),
	m41(0.0f), m42(0.0f), m43(0.0f), m44(0.0f) {}

Mat4x4::Mat4x4(
	float m11, float m12, float m13, float m14,
	float m21, float m22, float m23, float m24,
	float m31, float m32, float m33, float m34,
	float m41, float m42, float m43, float m44) :
	m11(m11), m12(m12), m13(m13), m14(m14),
	m21(m21), m22(m22), m23(m23), m24(m24),
	m31(m31), m32(m32), m33(m33), m34(m34),
	m41(m41), m42(m42), m43(m43), m44(m44) {}

const Mat4x4 Mat4x4::Identity = Mat4x4(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

Mat4x4 Mat4x4::CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane)
{
	Mat4x4 result = Identity;

	result.m11 = 2.0f / width;
	result.m12 = result.m13 = result.m14 = 0.0f;
	result.m22 = -2.0f / height;
	result.m21 = result.m23 = result.m24 = 0.0f;
	result.m33 = 1.0f / (zNearPlane - zFarPlane);
	result.m31 = result.m32 = result.m34 = 0.0f;
	result.m41 = result.m42 = 0.0f;
	result.m43 = zNearPlane / (zNearPlane - zFarPlane);
	result.m44 = 1.0f;

	return result;
}

Mat4x4 Mat4x4::CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
{
	Mat4x4 result = Identity;

	result.m11 = 2.0f / (right - left);
	result.m12 = result.m13 = result.m14 = 0.0f;
	result.m22 = 2.0f / (top - bottom);
	result.m21 = result.m23 = result.m24 = 0.0f;
	result.m33 = 1.0f / (zNearPlane - zFarPlane);
	result.m31 = result.m32 = result.m34 = 0.0f;
	result.m41 = (left + right) / (left - right);
	result.m42 = (top + bottom) / (bottom - top);
	result.m43 = zNearPlane / (zNearPlane - zFarPlane);
	result.m44 = 1.0f;

	return result;
}

Mat4x4 Mat4x4::CreateTranslation(float xPosition, float yPosition, float zPosition)
{
	Mat4x4 result = Identity;

	result.m41 = xPosition;
	result.m42 = yPosition;
	result.m43 = zPosition;

	return result;
}

Mat4x4 Mat4x4::CreateScale(float xScale, float yScale, float zScale)
{
	Mat4x4 result = Identity;

	result.m11 = xScale;
	result.m22 = yScale;
	result.m33 = zScale;

	return result;
}

Mat4x4 Mat4x4::operator*(const Mat4x4& rhs)
{
	Mat4x4 m;

	m.m11 = m11 * rhs.m11 + m12 * rhs.m21 + m13 * rhs.m31 + m14 * rhs.m41;
	m.m12 = m11 * rhs.m12 + m12 * rhs.m22 + m13 * rhs.m32 + m14 * rhs.m42;
	m.m13 = m11 * rhs.m13 + m12 * rhs.m23 + m13 * rhs.m33 + m14 * rhs.m43;
	m.m14 = m11 * rhs.m14 + m12 * rhs.m24 + m13 * rhs.m34 + m14 * rhs.m44;

	m.m21 = m21 * rhs.m11 + m22 * rhs.m21 + m23 * rhs.m31 + m24 * rhs.m41;
	m.m22 = m21 * rhs.m12 + m22 * rhs.m22 + m23 * rhs.m32 + m24 * rhs.m42;
	m.m23 = m21 * rhs.m13 + m22 * rhs.m23 + m23 * rhs.m33 + m24 * rhs.m43;
	m.m24 = m21 * rhs.m14 + m22 * rhs.m24 + m23 * rhs.m34 + m24 * rhs.m44;

	m.m31 = m31 * rhs.m11 + m32 * rhs.m21 + m33 * rhs.m31 + m34 * rhs.m41;
	m.m32 = m31 * rhs.m12 + m32 * rhs.m22 + m33 * rhs.m32 + m34 * rhs.m42;
	m.m33 = m31 * rhs.m13 + m32 * rhs.m23 + m33 * rhs.m33 + m34 * rhs.m43;
	m.m34 = m31 * rhs.m14 + m32 * rhs.m24 + m33 * rhs.m34 + m34 * rhs.m44;

	m.m41 = m41 * rhs.m11 + m42 * rhs.m21 + m43 * rhs.m31 + m44 * rhs.m41;
	m.m42 = m41 * rhs.m12 + m42 * rhs.m22 + m43 * rhs.m32 + m44 * rhs.m42;
	m.m43 = m41 * rhs.m13 + m42 * rhs.m23 + m43 * rhs.m33 + m44 * rhs.m43;
	m.m44 = m41 * rhs.m14 + m42 * rhs.m24 + m43 * rhs.m34 + m44 * rhs.m44;

	return m;
}