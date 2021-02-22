#pragma once

namespace Blah
{
	struct Vec3
	{
		float x;
		float y;
		float z;

		Vec3()
			: x(0)
			, y(0)
			, z(0)
		{}

		Vec3(float x, float y, float z)
			: x(x)
			, y(y)
			, z(z)
		{}
	};
}