#include <blah/numerics/mat3x2.h>
#include <blah/numerics/vec2.h>
#include <blah/numerics/calc.h>
#include <cstring>

using namespace Blah;

float Mat3x2::scaling_factor() const
{
	return Calc::sqrt(m11 * m11 + m12 * m12);
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
	float c = Calc::cos(radians);
	float s = Calc::sin(radians);

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