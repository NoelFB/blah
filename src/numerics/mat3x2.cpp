#include <blah/numerics/mat3x2.h>
#include <blah/numerics/vec2.h>
#include <blah/numerics/calc.h>

using namespace Blah;

float Mat3x2::scaling_factor() const
{
	return Calc::sqrt(m11 * m11 + m12 * m12);
}

Mat3x2 Mat3x2::create_translation(const Vec2& position)
{
	return create_translation(position.x, position.y);
}

Mat3x2 Mat3x2::create_scale(const Vec2& scale)
{
	return create_scale(scale.x, scale.y);
}

Mat3x2 Mat3x2::create_scale(float scale, const Vec2& center_point)
{
	Mat3x2 result;

	float tx = center_point.x * (1 - scale);
	float ty = center_point.y * (1 - scale);

	result.m11 = scale;
	result.m12 = 0.0f;
	result.m21 = 0.0f;
	result.m22 = scale;
	result.m31 = tx;
	result.m32 = ty;

	return result;
}

Mat3x2 Mat3x2::create_scale(const Vec2& scale, const Vec2& center_point)
{
	Mat3x2 result;

	float tx = center_point.x * (1 - scale.x);
	float ty = center_point.y * (1 - scale.y);

	result.m11 = scale.x;
	result.m12 = 0.0f;
	result.m21 = 0.0f;
	result.m22 = scale.y;
	result.m31 = tx;
	result.m32 = ty;

	return result;
}

Mat3x2 Mat3x2::create_scale(float scale_x, float scale_y, const Vec2& center_point)
{
	Mat3x2 result;

	float tx = center_point.x * (1 - scale_x);
	float ty = center_point.y * (1 - scale_y);

	result.m11 = scale_x;
	result.m12 = 0.0f;
	result.m21 = 0.0f;
	result.m22 = scale_y;
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