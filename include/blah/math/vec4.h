#pragma once

namespace Blah
{
	struct Vec4
	{
		float x;
		float y;
		float z;
		float w;

		Vec4() : x(0), y(0), z(0), w(0) {}
		Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	};
}