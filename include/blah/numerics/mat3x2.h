#pragma once

namespace Blah
{
	struct Vec2;

	struct Mat3x2
	{
		float m11;
		float m12;
		float m21;
		float m22;
		float m31;
		float m32;

		constexpr Mat3x2()
			: m11(0), m12(0), m21(0), m22(0), m31(0), m32(0) {}

		constexpr Mat3x2(float m11, float m12, float m21, float m22, float m31, float m32)
			: m11(m11), m12(m12), m21(m21), m22(m22), m31(m31), m32(m32) {}

		static const Mat3x2 identity;

		constexpr Mat3x2 invert() const
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

		float scaling_factor() const;

	 	static Mat3x2 create_translation(const Vec2& position);

		static constexpr Mat3x2 create_translation(float x, float y)
		{
			return Mat3x2(1, 0, 0, 1, x, y);
		}

		static constexpr Mat3x2 create_scale(float scale)
		{
			return Mat3x2(scale, 0, 0, scale, 0, 0);
		}

		static Mat3x2 create_scale(float x, float y)
		{
			return Mat3x2(x, 0, 0, y, 0, 0);
		}

		static Mat3x2 create_scale(const Vec2& scale);
		static Mat3x2 create_scale(float scale, const Vec2& center_point);
		static Mat3x2 create_scale(const Vec2& scale, const Vec2& center_point);
		static Mat3x2 create_scale(float scale_x, float scale_y, const Vec2& center_point);

		static Mat3x2 create_rotation(float radians);

		static Mat3x2 create_transform(const Vec2& position, const Vec2& origin, const Vec2& scale, float rotation);

		static constexpr Mat3x2 add(const Mat3x2& a, const Mat3x2& b)
		{
			return Mat3x2(
				a.m11 + b.m11,
				a.m12 + b.m12,
				a.m21 + b.m21,
				a.m22 + b.m22,
				a.m31 + b.m31,
				a.m32 + b.m32);
		}

		static constexpr Mat3x2 subtract(const Mat3x2& a, const Mat3x2& b)
		{
			return Mat3x2(
				a.m11 - b.m11,
				a.m12 - b.m12,
				a.m21 - b.m21,
				a.m22 - b.m22,
				a.m31 - b.m31,
				a.m32 - b.m32);
		}

		static constexpr Mat3x2 multiply(const Mat3x2& a, const Mat3x2& b)
		{
			return Mat3x2(a.m11 * b.m11 + a.m12 * b.m21,
				a.m11 * b.m12 + a.m12 * b.m22,
				a.m21 * b.m11 + a.m22 * b.m21,
				a.m21 * b.m12 + a.m22 * b.m22,
				a.m31 * b.m11 + a.m32 * b.m21 + b.m31,
				a.m31 * b.m12 + a.m32 * b.m22 + b.m32);
		}

		constexpr Mat3x2 operator *(const Mat3x2& rhs) const
		{
			return multiply(*this, rhs);
		}

		constexpr Mat3x2 operator +(const Mat3x2& rhs) const
		{
			return add(*this, rhs);
		}

		constexpr Mat3x2 operator -(const Mat3x2& rhs) const
		{
			return subtract(*this, rhs);
		}

		constexpr Mat3x2& operator *=(const Mat3x2& rhs)
		{
			*this = multiply(*this, rhs);
			return *this;
		}

		constexpr bool operator==(const Mat3x2& rhs)
		{
			return m11 == rhs.m11 && m12 == rhs.m12 && m21 == rhs.m21 && m22 == rhs.m22 && m31 == rhs.m31 && m32 == rhs.m32;
		}

		constexpr bool operator!=(const Mat3x2& rhs)
		{
			return m11 != rhs.m11 || m12 != rhs.m12 || m21 != rhs.m21 || m22 != rhs.m22 || m31 != rhs.m31 || m32 != rhs.m32;
		}
	};

	inline const Mat3x2 Mat3x2::identity = Mat3x2(1, 0, 0, 1, 0, 0);
}