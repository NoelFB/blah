#include <blah/math/quad.h>
#include <blah/math/calc.h>

namespace Blah
{
	void Quad::project(const Vec2& axis, float* min, float* max) const
	{
		float dot = Vec2::dot(a, axis);
		*min = dot;
		*max = dot;
		dot = Vec2::dot(b, axis);
		*min = Calc::min(dot, *min);
		*max = Calc::max(dot, *max);
		dot = Vec2::dot(c, axis);
		*min = Calc::min(dot, *min);
		*max = Calc::max(dot, *max);
		dot = Vec2::dot(d, axis);
		*min = Calc::min(dot, *min);
		*max = Calc::max(dot, *max);
	}
}