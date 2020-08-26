#pragma once

namespace Blah
{
	struct Mat4x4
	{
		float m11;
		float m12;
		float m13;
		float m14;

		float m21;
		float m22;
		float m23;
		float m24;

		float m31;
		float m32;
		float m33;
		float m34;

		float m41;
		float m42;
		float m43;
		float m44;

		Mat4x4();
		Mat4x4(
			float m11, float m12, float m13, float m14,
			float m21, float m22, float m23, float m24,
			float m31, float m32, float m33, float m34,
			float m41, float m42, float m43, float m44);


		static const Mat4x4 Identity;

		static Mat4x4 CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane);
		static Mat4x4 CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);
		static Mat4x4 CreateTranslation(float xPosition, float yPosition, float zPosition);
		static Mat4x4 CreateScale(float xScale, float yScale, float zScale);

		Mat4x4 operator* (const Mat4x4& rhs);
	};
}