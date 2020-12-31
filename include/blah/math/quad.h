#pragma once
#include <blah/math/vec2.h>

namespace Blah
{
	struct Quad
	{
		Vec2 a;
		Vec2 b;
		Vec2 c;
		Vec2 d;

		Quad() {}
		Quad(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
			: a(a), b(b), c(c), d(d) {}

		void project(const Vec2& axis, float* min, float* max) const;
	};
}