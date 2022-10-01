#pragma once
#include <blah_common.h>
#include <blah_calc.h>

namespace Blah
{
	static constexpr double epsilon = 0.000001;

	template<class T> struct Vec2;
	template<class T> struct Vec3;
	template<class T> struct Vec4;
	template<class T> struct Mat3x2;
	template<class T> struct Mat4x4;
	template<class T> struct Rect;
	template<class T> struct Quad;
	template<class T> struct Line;
	template<class T> struct Circle;

	using Vec2f = Vec2<float>;
	using Vec2d = Vec2<double>;
	using Vec2i = Vec2<int>;
	using Point = Vec2<int>;
	using Vec3f = Vec3<float>;
	using Vec3d = Vec3<double>;
	using Vec4f = Vec4<float>;
	using Vec4d = Vec4<double>;
	using Mat3x2f = Mat3x2<float>;
	using Mat3x2d = Mat3x2<double>;
	using Mat4x4f = Mat4x4<float>;
	using Mat4x4d = Mat4x4<double>;
	using Rectf = Rect<float>;
	using Rectd = Rect<double>;
	using Recti = Rect<int>;
	using Quadf = Quad<float>;
	using Quadd = Quad<double>;
	using Linef = Line<float>;
	using Lined = Line<double>;
	using Circlef = Circle<float>;
	using Circled = Circle<double>;

	template<class T>
	struct Vec2
	{
		T x;
		T y;

		constexpr Vec2();

		template<class X, class Y>
		constexpr Vec2(X x, Y y);

		template<class K>
		constexpr Vec2(const Vec2<K>& vector);

		constexpr Vec2 operator+(const Vec2& rhs) const;
		constexpr Vec2 operator-(const Vec2& rhs) const;
		constexpr Vec2 operator/(const T rhs) const;
		constexpr Vec2 operator*(const T rhs) const;
		constexpr Vec2 operator*(const Vec2& rhs);
		constexpr Vec2 operator-() const;
		constexpr Vec2& operator+=(const Vec2& rhs);
		constexpr Vec2& operator-=(const Vec2& rhs);
		constexpr Vec2& operator/=(const Vec2& rhs);
		constexpr Vec2& operator*=(const Vec2& rhs);
		constexpr Vec2& operator/=(T rhs);
		constexpr Vec2& operator*=(T rhs);
		constexpr bool operator==(const Vec2& rhs) const;
		constexpr bool operator!=(const Vec2& rhs) const;

		constexpr Vec2 abs() const;
		Vec2 normal() const;
		constexpr Vec2 turn_right() const;
		constexpr Vec2 turn_left() const;
		T length() const;
		constexpr T length_squared() const;
		T angle() const;
		static constexpr T dot(const Vec2& a, const Vec2& b);
		static constexpr Vec2 transform(const Vec2& vec, const Mat3x2<T>& matrix);
		static constexpr Vec2 transform_normal(const Vec2& vec, const Mat3x2<T>& matrix);
		static Vec2 from_angle(T radians, T length = 1);
		static constexpr Vec2 approach(const Vec2& t, const Vec2& target, T delta);
		static constexpr Vec2 lerp(const Vec2& a, const Vec2& b, T t);
		static constexpr Vec2 lerp_bezier(const Vec2& a, const Vec2& b, const Vec2& end, T t);
		static constexpr Vec2 lerp_bezier(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& end, T t);
		static constexpr Vec2 reflect(const Vec2& vector, const Vec2& normal);
		static constexpr Vec2 min(const Vec2& a, const Vec2& b);
		static constexpr Vec2 max(const Vec2& a, const Vec2& b);

		static const Vec2 unit_x;
		static const Vec2 unit_y;
		static const Vec2 right;
		static const Vec2 up;
		static const Vec2 down;
		static const Vec2 left;
		static const Vec2 zero;
		static const Vec2 one;
	};

	template<class T> const Vec2<T> Vec2<T>::unit_x = Vec2<T>(1, 0);
	template<class T> const Vec2<T> Vec2<T>::unit_y = Vec2<T>(0, 1);
	template<class T> const Vec2<T> Vec2<T>::right = Vec2<T>(1, 0);
	template<class T> const Vec2<T> Vec2<T>::up = Vec2<T>(0, -1);
	template<class T> const Vec2<T> Vec2<T>::down = Vec2<T>(0, 1);
	template<class T> const Vec2<T> Vec2<T>::left = Vec2<T>(-1, 0);
	template<class T> const Vec2<T> Vec2<T>::zero = Vec2<T>(0, 0);
	template<class T> const Vec2<T> Vec2<T>::one = Vec2<T>(1, 1);

	template<class T>
	struct Vec3
	{
		T x;
		T y;
		T z;

		constexpr Vec3();

		template<class X, class Y, class Z>
		constexpr Vec3(X x, Y y, Z z);

		template<class K>
		constexpr Vec3(const Vec3<K>& vector);

		constexpr Vec3 operator +(const Vec3& rhs) const;
		constexpr Vec3 operator -(const Vec3& rhs) const;

		T length() const;
		Vec3 normal() const;

		static constexpr T dot(const Vec3& a, const Vec3& b);
		static constexpr Vec3 cross(const Vec3& a, const Vec3& b);
	};
	
	template<class T>
	struct Vec4
	{
		T x;
		T y;
		T z;
		T w;

		constexpr Vec4();

		template<class X, class Y, class Z, class W>
		constexpr Vec4(X x, Y y, Z z, W w);

		template<class K>
		constexpr Vec4(const Vec4<K>& vector);
	};

	template<class T>
	struct Rect
	{
		T x;
		T y;
		T w;
		T h;

		constexpr Rect();
		constexpr Rect(T x, T y, T w, T h);

		template<class X, class Y, class W, class H>
		constexpr Rect(X x, Y y, W w, H h);

		template<class K>
		constexpr Rect(const Rect<K>& rect);

		constexpr Rect operator+(const Vec2<T>& rhs) const;
		constexpr Rect operator-(const Vec2<T>& rhs) const;
		constexpr Rect& operator+=(const Vec2<T>& rhs);
		constexpr Rect& operator-=(const Vec2<T>& rhs);
		constexpr bool operator==(const Rect& rhs) const;
		constexpr bool operator!=(const Rect& rhs) const;

		constexpr T left() const;
		constexpr T right() const;
		constexpr T top() const;
		constexpr T bottom() const;
		constexpr Vec2<T> center() const;
		constexpr T center_x() const;
		constexpr T center_y() const;
		constexpr Vec2<T> top_left() const;
		constexpr Vec2<T> top_right() const;
		constexpr Vec2<T> bottom_right() const;
		constexpr Vec2<T> bottom_left() const;
		constexpr Vec2<T> center_left() const;
		constexpr Vec2<T> center_right() const;
		constexpr Vec2<T> middle_top() const;
		constexpr Vec2<T> middle_bottom() const;
		constexpr Line<T> left_line() const;
		constexpr Line<T> right_line() const;
		constexpr Line<T> top_line() const;
		constexpr Line<T> bottom_line() const;

		constexpr bool contains(const Vec2<T>& pt) const;
		constexpr bool overlaps(const Rect& rect) const;
		constexpr Rect overlap_rect(const Rect& other) const;
		constexpr bool intersects(const Line<T>& line) const;
		constexpr bool intersects(const Line<T>& line, Vec2<T>* out_intersection_point) const;
		constexpr bool intersects(const Vec2<T>& line_from, const Vec2<T>& line_to) const;
		constexpr bool intersects(const Vec2<T>& line_from, const Vec2<T>& line_to, Vec2<T>* out_intersection_point) const;
		constexpr Vec2<T> intersection_point(const Line<T>& line) const;
		constexpr Vec2<T> intersection_point(const Vec2<T>& line_from, const Vec2<T>& line_to) const;

		constexpr Rect scale(T s) const;
		constexpr Rect scale(T sx, T sy) const;
		constexpr Rect inflate(T amount) const;
		constexpr Rect inflate(T amount_x, T amount_y) const;

		// Rect Sectors:
		//		0101  0100  0110
		//		0001  0000  0010
		//		1001  1000  1010
		//	0000 = inside rectangle, all others refer to sectors relative to the rectangle
		constexpr u8 get_sector(const Vec2<T>& pt) const;

		static constexpr Rect transform(const Rect& rect, const Mat3x2<T>& matrix);
		static constexpr Rect from_points(const Vec2<T>& from, const Vec2<T>& to);
	};

	template<class T>
	struct Circle
	{
		Vec2<T> center;
		T radius;

		constexpr Circle();
		constexpr Circle(T x, T y, T radius);
		constexpr Circle(const Vec2<T>& center, T radius);

		template<class K>
		constexpr Circle(const Circle<K>& circle);

		constexpr void project(const Vec2<T>& axis, T* min, T* max) const;
	};

	template<class T>
	struct Quad
	{
		Vec2<T> a;
		Vec2<T> b;
		Vec2<T> c;
		Vec2<T> d;

		constexpr Quad();
		constexpr Quad(const Vec2<T>& a, const Vec2<T>& b, const Vec2<T>& c, const Vec2<T>& d);

		constexpr void project(const Vec2<T>& axis, T* min, T* max) const;
	};

	template<class T>
	struct Line
	{
		Vec2<T> a;
		Vec2<T> b;

		constexpr Line();
		constexpr Line(const Vec2<T>& a, const Vec2<T>& b);
		constexpr Line(T x0, T y0, T x1, T y1);

		constexpr Rect<T> bounds() const;
		constexpr Vec2<T> closest_point(const Vec2<T>& pt) const;
		constexpr bool intersects(const Rect<T>& rect, Vec2<T>* out_intersection_point = nullptr) const;
		constexpr bool intersects(const Line<T>& line, Vec2<T>* out_intersection_point = nullptr) const;

		constexpr void project(const Vec2<T>& axis, T* min, T* max) const;

		constexpr Line operator +(const Vec2<T>& rhs) const;
		constexpr Line operator -(const Vec2<T>& rhs) const;
	};

	template<class T>
	struct Mat3x2
	{
		T m11;
		T m12;
		T m21;
		T m22;
		T m31;
		T m32;

		constexpr Mat3x2();
		constexpr Mat3x2(T m11, T m12, T m21, T m22, T m31, T m32);

		template<class K>
		constexpr Mat3x2(const Mat3x2<K>& matrix);

		static const Mat3x2 identity;

		constexpr Mat3x2 invert() const;
		T scaling_factor() const;

		static constexpr Mat3x2 create_translation(const Vec2<T>& position);
		static constexpr Mat3x2 create_translation(T x, T y);
		static constexpr Mat3x2 create_scale(T scale);
		static constexpr Mat3x2 create_scale(T x, T y);
		static constexpr Mat3x2 create_scale(const Vec2<T>& scale);
		static constexpr Mat3x2 create_scale(T scale, const Vec2<T>& center_point);
		static constexpr Mat3x2 create_scale(const Vec2<T>& scale, const Vec2<T>& center_point);
		static constexpr Mat3x2 create_scale(T scale_x, T scale_y, const Vec2<T>& center_point);
		static Mat3x2 create_rotation(T radians);
		static Mat3x2 create_transform(const Vec2<T>& position, const Vec2<T>& origin, const Vec2<T>& scale, T rotation);
		static constexpr Mat3x2 add(const Mat3x2& a, const Mat3x2& b);
		static constexpr Mat3x2 subtract(const Mat3x2& a, const Mat3x2& b);
		static constexpr Mat3x2 multiply(const Mat3x2& a, const Mat3x2& b);

		constexpr Mat3x2 operator *(const Mat3x2& rhs) const;
		constexpr Mat3x2 operator +(const Mat3x2& rhs) const;
		constexpr Mat3x2 operator -(const Mat3x2& rhs) const;
		constexpr Mat3x2& operator *=(const Mat3x2& rhs);
		constexpr bool operator==(const Mat3x2& rhs);
		constexpr bool operator!=(const Mat3x2& rhs);
	};

	template<class T> const Mat3x2<T> Mat3x2<T>::identity = Mat3x2<T>(1, 0, 0, 1, 0, 0);

	template<class T>
	struct Mat4x4
	{
		T m11;
		T m12;
		T m13;
		T m14;

		T m21;
		T m22;
		T m23;
		T m24;

		T m31;
		T m32;
		T m33;
		T m34;

		T m41;
		T m42;
		T m43;
		T m44;

		Mat4x4();
		Mat4x4(
			T m11, T m12, T m13, T m14,
			T m21, T m22, T m23, T m24,
			T m31, T m32, T m33, T m34,
			T m41, T m42, T m43, T m44);

		static const Mat4x4 identity;

		static constexpr Mat4x4 create_ortho(T width, T height, T z_near_plane, T z_far_plane);
		static constexpr Mat4x4 create_ortho_offcenter(T left, T right, T bottom, T top, T z_near_plane, T z_far_plane);
		static Mat4x4 create_perspective(T field_of_view, T ratio, T z_near_plane, T z_far_plane);
		static constexpr Mat4x4 create_translation(T x, T y, T z);
		static constexpr Mat4x4 create_scale(T x, T y, T z);
		static Mat4x4 create_lookat(const Vec3<T>& position, const Vec3<T>& target, const Vec3<T>& up);

		constexpr Mat4x4 operator* (const Mat4x4& rhs);
	};

	template<class T> const Mat4x4<T> Mat4x4<T>::identity = Mat4x4<T>(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	template<class T>
	constexpr Vec2<T>::Vec2()
		: x(0), y(0) {}

	template<class T>
	template<class X, class Y>
	constexpr Vec2<T>::Vec2(X x, Y y)
		: x(static_cast<T>(x)), y(static_cast<T>(y)) {}
	
	template<class T>
	template<class K>
	constexpr Vec2<T>::Vec2(const Vec2<K>& vector)
		: x(static_cast<T>(vector.x)), y(static_cast<T>(vector.y)) {}

	template<class T>
	constexpr Vec2<T> Vec2<T>::operator+(const Vec2& rhs) const {
		return Vec2(x + rhs.x, y + rhs.y);
	}
	template<class T>
	constexpr Vec2<T> Vec2<T>::operator-(const Vec2& rhs) const {
		return Vec2(x - rhs.x, y - rhs.y);
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::operator-() const {
		return Vec2(-x, -y);
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::operator/(const T rhs) const {
		return Vec2(x / rhs, y / rhs);
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::operator* (const T rhs) const {
		return Vec2(x * rhs, y * rhs);
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::operator *(const Vec2& rhs) {
		return Vec2(x * rhs.x, y * rhs.y);
	}

	template<class T>
	constexpr Vec2<T>& Vec2<T>::operator += (const Vec2& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	template<class T>
	constexpr Vec2<T>& Vec2<T>::operator -= (const Vec2& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	template<class T>
	constexpr Vec2<T>& Vec2<T>::operator /= (T rhs) {
		x /= rhs;
		y /= rhs;
		return *this;
	}

	template<class T>
	constexpr Vec2<T>& Vec2<T>::operator *= (const Vec2 & rhs) {
		x *= rhs.x;
		y *= rhs.y;
		return this;
	}

	template<class T>
	constexpr Vec2<T>& Vec2<T>::operator *=(T rhs) {
		x *= rhs;
		y *= rhs;
		return *this;
	}

	template<class T>
	constexpr bool Vec2<T>::operator ==(const Vec2& rhs) const {
		return Calc::abs(x - rhs.x) < epsilon && Calc::abs(y - rhs.y) < epsilon;
	}

	template<class T>
	constexpr bool Vec2<T>::operator !=(const Vec2& rhs) const {
		return !(*this == rhs);
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::abs() const {
		return Vec2(Calc::abs(x), Calc::abs(y));
	}

	template<class T>
	Vec2<T> Vec2<T>::normal() const {
		auto len = Calc::sqrt(x * x + y * y);
		if (len <= 0)
			return Vec2<T>(0, 0);
		return Vec2(x / len, y / len);
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::turn_right() const {
		return Vec2(-y, x);
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::turn_left() const {
		return Vec2(y, -x);
	}

	template<class T>
	T Vec2<T>::length() const {
		return static_cast<T>(Calc::sqrt(static_cast<f32>(x * x + y * y)));
	}

	template<class T>
	constexpr T Vec2<T>::length_squared() const {
		return x * x + y * y;
	}

	template<class T>
	T Vec2<T>::angle() const {
		return Calc::atan2(y, x);
	}

	template<class T>
	constexpr T Vec2<T>::dot(const Vec2& a, const Vec2& b) {
		return a.x * b.x + a.y * b.y;
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::transform(const Vec2& vec, const Mat3x2<T>& matrix) {
		return Vec2(
			(vec.x * matrix.m11) + (vec.y * matrix.m21) + matrix.m31,
			(vec.x * matrix.m12) + (vec.y * matrix.m22) + matrix.m32);
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::transform_normal(const Vec2& vec, const Mat3x2<T>& matrix) {
		return Vec2(
			vec.x * matrix.m11 + vec.y * matrix.m21,
			vec.x * matrix.m12 + vec.y * matrix.m22);
	}

	template<class T>
	Vec2<T> Vec2<T>::from_angle(T radians, T length) {
		return Vec2(
			Calc::cos(radians) * length,
			Calc::sin(radians) * length);
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::approach(const Vec2& value, const Vec2& target, T delta) {
		if ((value - target).length_squared() <= delta * delta)
			return target;
		return value + (target - value).normal() * delta;
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::lerp(const Vec2& a, const Vec2& b, T t) {
		if (t == 0)
			return a;
		else if (t == 1)
			return b;
		else
			return a + (b - a) * t;
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::lerp_bezier(const Vec2& a, const Vec2& b, const Vec2& end, T t) {
		return lerp(lerp(a, b, t), lerp(b, end, t), t);
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::lerp_bezier(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& end, T t) {
		return lerp_bezier(lerp(a, b, t), lerp(b, c, t), lerp(c, end, t), t);
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::reflect(const Vec2& vector, const Vec2& normal) {
		auto dot = vector.x * normal.x + vector.y * normal.y;
		return Vec2(
			vector.x - 2 * dot * normal.x,
			vector.y - 2 * dot * normal.y);
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::min(const Vec2& a, const Vec2& b) {
		return Vec2(Calc::min(a.x, b.x), Calc::min(a.y, b.y));
	}

	template<class T>
	constexpr Vec2<T> Vec2<T>::max(const Vec2& a, const Vec2& b) {
		return Vec2(Calc::max(a.x, b.x), Calc::max(a.y, b.y));
	}

	template<class T>
	constexpr Vec3<T>::Vec3()
		: x(0), y(0), z(0) {}

	template<class T>
	template<class X, class Y, class Z>
	constexpr Vec3<T>::Vec3(X x, Y y, Z z)
		: x(static_cast<T>(x)), y(static_cast<T>(y)), z(static_cast<T>(z)) {}

	template<class T>
	template<class K>
	constexpr Vec3<T>::Vec3(const Vec3<K>& vector)
		: x(static_cast<T>(vector.x)), y(static_cast<T>(vector.y)), z(static_cast<T>(vector.z)) {}

	template<class T>
	constexpr Vec3<T> Vec3<T>::operator+(const Vec3& rhs) const {
		return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	template<class T>
	constexpr Vec3<T> Vec3<T>::operator-(const Vec3& rhs) const {
		return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	template<class T>
	T Vec3<T>::length() const {
		return Calc::sqrt(x * x + y * y + z * z);
	}

	template<class T>
	Vec3<T> Vec3<T>::normal() const {
		T len = length();
		return Vec3(x / len, y / len, z / len);
	}

	template<class T>
	constexpr T Vec3<T>::dot(const Vec3& a, const Vec3& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	template<class T>
	constexpr Vec3<T> Vec3<T>::cross(const Vec3& a, const Vec3& b) {
		return Vec3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x);
	}

	template<class T>
	constexpr Vec4<T>::Vec4()
		: x(0), y(0), z(0), w(0) {}

	template<class T>
	template<class X, class Y, class Z, class W>
	constexpr Vec4<T>::Vec4(X x, Y y, Z z, W w)
		: x(static_cast<T>(x)), y(static_cast<T>(y)), z(static_cast<T>(z)), w(static_cast<T>(w)) {}

	template<class T>
	template<class K>
	constexpr Vec4<T>::Vec4(const Vec4<K>& vector)
		: x(static_cast<T>(vector.x)), y(static_cast<T>(vector.y)), z(static_cast<T>(vector.z)), w(static_cast<T>(vector.w)) {}

	template<class T>
	constexpr Rect<T>::Rect()
		: x(0), y(0), w(0), h(0) {}

	template<class T>
	constexpr Rect<T>::Rect(T x, T y, T w, T h)
		: x(x), y(y), w(w), h(h) {}

	template<class T>
	template<class X, class Y, class W, class H>
	constexpr Rect<T>::Rect(X x, Y y, W w, H h)
		: x(static_cast<T>(x)), y(static_cast<T>(y)), w(static_cast<T>(w)), h(static_cast<T>(h)) {}

	template<class T>
	template<class K>
	constexpr Rect<T>::Rect(const Rect<K>& rect)
		: x(static_cast<T>(rect.x)), y(static_cast<T>(rect.y)), w(static_cast<T>(rect.w)), h(static_cast<T>(rect.h)) {}

	template<class T>
	constexpr Rect<T> Rect<T>::operator+(const Vec2<T>& rhs) const {
		return Rect(x + rhs.x, y + rhs.y, w, h);
	}

	template<class T>
	constexpr Rect<T> Rect<T>::operator-(const Vec2<T>& rhs) const {
		return Rect(x - rhs.x, y - rhs.y, w, h);
	}

	template<class T>
	constexpr Rect<T>& Rect<T>::operator+=(const Vec2<T>& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	template<class T>
	constexpr Rect<T>& Rect<T>::operator-=(const Vec2<T>& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	template<class T>
	constexpr bool Rect<T>::operator==(const Rect& rhs) const {
		return Calc::abs(x - rhs.x) < epsilon && Calc::abs(y - rhs.y) < epsilon && Calc::abs(w - rhs.w) < epsilon && Calc::abs(h - rhs.h) < epsilon;
	}

	template<class T>
	constexpr bool Rect<T>::operator!=(const Rect& rhs) const {
		return !(*this == rhs);
	}

	template<class T>
	constexpr T Rect<T>::left() const { return x; }
	template<class T>
	constexpr T Rect<T>::right() const { return x + w; }
	template<class T>
	constexpr T Rect<T>::top() const { return y; }
	template<class T>
	constexpr T Rect<T>::bottom() const { return y + h; }

	template<class T>
	constexpr Vec2<T> Rect<T>::center() const { return Vec2<T>(x + w / 2, y + h / 2); }
	template<class T>
	constexpr T Rect<T>::center_x() const { return x + w / 2; }
	template<class T>
	constexpr T Rect<T>::center_y() const { return y + h / 2; }

	template<class T>
	constexpr Vec2<T> Rect<T>::top_left() const { return Vec2<T>(x, y); }
	template<class T>
	constexpr Vec2<T> Rect<T>::top_right() const { return Vec2<T>(x + w, y); }
	template<class T>
	constexpr Vec2<T> Rect<T>::bottom_right() const { return Vec2<T>(x + w, y + h); }
	template<class T>
	constexpr Vec2<T> Rect<T>::bottom_left() const { return Vec2<T>(x, y + h); }

	template<class T>
	constexpr Vec2<T> Rect<T>::center_left() const { return Vec2<T>(x, y + h / 2); }
	template<class T>
	constexpr Vec2<T> Rect<T>::center_right() const { return Vec2<T>(x + w, y + h / 2); }
	template<class T>
	constexpr Vec2<T> Rect<T>::middle_top() const { return Vec2<T>(x + w / 2, y); }
	template<class T>
	constexpr Vec2<T> Rect<T>::middle_bottom() const { return Vec2<T>(x + w / 2, y + h); }

	template<class T>
	constexpr Line<T> Rect<T>::left_line() const { return Line<T>(left(), top(), left(), bottom()); }
	template<class T>
	constexpr Line<T> Rect<T>::right_line() const { return Line<T>(right(), top(), right(), bottom()); }
	template<class T>
	constexpr Line<T> Rect<T>::top_line() const { return Line<T>(left(), top(), right(), top()); }
	template<class T>
	constexpr Line<T> Rect<T>::bottom_line() const { return Line<T>(left(), bottom(), right(), bottom()); }

	template<class T>
	constexpr bool Rect<T>::contains(const Vec2<T>& pt) const {
		return pt.x >= x && pt.x < x + w && pt.y >= y && pt.y < y + h;
	}

	template<class T>
	constexpr bool Rect<T>::overlaps(const Rect& rect) const {
		return x + w >= rect.x && y + h >= rect.y && x < rect.x + rect.w && y < rect.y + rect.h;
	}

	template<class T>
	constexpr Rect<T> Rect<T>::overlap_rect(const Rect& against) const {
		Rect result;

		if (x + w >= against.x && x < against.x + against.w)
		{
			result.x = Calc::max(x, against.x);
			result.w = Calc::min(x + w, against.x + against.w) - result.x;
		}

		if (y + h >= against.y && y < against.y + against.h)
		{
			result.y = Calc::max(y, against.y);
			result.h = Calc::min(y + h, against.y + against.h) - result.y;
		}

		return result;
	}

	template<class T>
	constexpr bool Rect<T>::intersects(const Line<T>& line) const {
		return line.intersects(*this);
	}

	template<class T>
	constexpr bool Rect<T>::intersects(const Line<T>& line, Vec2<T>* out_intersection_point) const {
		return line.intersects(*this, out_intersection_point);
	}

	template<class T>
	constexpr bool Rect<T>::intersects(const Vec2<T>& line_from, const Vec2<T>& line_to) const {
		return intersects(Line<T>(line_from, line_to));
	}

	template<class T>
	constexpr bool Rect<T>::intersects(const Vec2<T>& line_from, const Vec2<T>& line_to, Vec2<T>* out_intersection_point) const {
		return intersects(Line<T>(line_from, line_to), out_intersection_point);
	}

	template<class T>
	constexpr Vec2<T> Rect<T>::intersection_point(const Line<T>& line) const {
		Vec2<T> ret;
		if (line.intersects(*this, &ret))
			return ret;
		return Vec2<T>::zero;
	}

	template<class T>
	constexpr Vec2<T> Rect<T>::intersection_point(const Vec2<T>& line_from, const Vec2<T>& line_to) const {
		Vec2<T> ret;
		if (Line<T>(line_from, line_to).intersects(*this, &ret))
			return ret;
		return Vec2<T>::zero;
	}

	template<class T>
	constexpr Rect<T> Rect<T>::scale(T s) const {
		return Rect<T>(x * s, y * s, w * s, h * s);
	}

	template<class T>
	constexpr Rect<T> Rect<T>::scale(T sx, T sy) const {
		return Rect<T>(x * sx, y * sy, w * sx, h * sy);
	}

	template<class T>
	constexpr Rect<T> Rect<T>::inflate(T amount) const {
		return Rect(x - amount, y - amount, w + amount * 2, h + amount * 2);
	}

	template<class T>
	constexpr Rect<T> Rect<T>::inflate(T amount_x, T amount_y) const {
		return Rect(x - amount_x, y - amount_y, w + amount_x * 2, h + amount_y * 2);
	}

	template<class T>
	constexpr u8 Rect<T>::get_sector(const Vec2<T>& pt) const
	{
		u8 result = 0;
		if (pt.x < left())
			result |= 0b0001;
		else if (pt.x >= right())
			result |= 0b0010;
		if (pt.y < top())
			result |= 0b0100;
		else if (pt.y >= bottom())
			result |= 0b1000;
		return result;
	}

	template<class T>
	constexpr Rect<T> Rect<T>::transform(const Rect& rect, const Mat3x2<T>& matrix) {
		return Rect<T>(
			(rect.x * matrix.m11) + (rect.y * matrix.m21) + matrix.m31,
			(rect.x * matrix.m12) + (rect.y * matrix.m22) + matrix.m32,
			(rect.w * matrix.m11) + (rect.h * matrix.m21),
			(rect.w * matrix.m12) + (rect.h * matrix.m22));
	}

	template<class T>
	constexpr Rect<T> Rect<T>::from_points(const Vec2<T>& from, const Vec2<T>& to) {
		auto min = Vec2<T>::min(from, to);
		auto max = Vec2<T>::max(from, to);
		return Rect<T>(min.x, min.y, max.x - min.x, max.y - min.y);
	}

	template<class T>
	constexpr Quad<T>::Quad()
		: a(), b(), c(), d() {}

	template<class T>
	constexpr Quad<T>::Quad(const Vec2<T>& a, const Vec2<T>& b, const Vec2<T>& c, const Vec2<T>& d)
		: a(a), b(b), c(c), d(d) {}

	template<class T>
	constexpr void Quad<T>::project(const Vec2<T>& axis, T* min, T* max) const {
		T dot = Vec2<T>::dot(a, axis);
		*min = dot;
		*max = dot;
		dot = Vec2<T>::dot(b, axis);
		*min = dot < *min ? dot : *min;
		*max = dot > *max ? dot : *max;
		dot = Vec2<T>::dot(c, axis);
		*min = dot < *min ? dot : *min;
		*max = dot > *max ? dot : *max;
		dot = Vec2<T>::dot(d, axis);
		*min = dot < *min ? dot : *min;
		*max = dot > *max ? dot : *max;
	}

	template<class T>
	constexpr Circle<T>::Circle()
		: center(0, 0), radius(0) {}
	template<class T>
	constexpr Circle<T>::Circle(T x, T y, T radius)
		: center(x, y), radius(radius) {}
	template<class T>
	constexpr Circle<T>::Circle(const Vec2<T>& center, T radius)
		: center(center), radius(radius) {}

	template<class T>
	template<class K>
	constexpr Circle<T>::Circle(const Circle<K>& circle)
		: center(Vec2<T>(static_cast<T>(circle.center.x), static_cast<T>(circle.center.y))), radius(static_cast<T>(circle.radius)) {}

	template<class T>
	constexpr void Circle<T>::project(const Vec2<T>& axis, T* min, T* max) const
	{
		*min = Vec2<T>::dot(center - axis * radius, axis);
		*max = Vec2<T>::dot(center + axis * radius, axis);
	}

	template<class T>
	constexpr Line<T>::Line()
		: a(0, 0), b(0, 0) {}

	template<class T>
	constexpr Line<T>::Line(T x0, T y0, T x1, T y1)
		: a(x0, y0), b(x1, y1) {}

	template<class T>
	constexpr Line<T>::Line(const Vec2<T>& a, const Vec2<T>& b)
		: a(a), b(b) {}

	template<class T>
	constexpr Rect<T> Line<T>::bounds() const {
		return Rect<T>::from_points(a, b);
	}

	template<class T>
	constexpr Vec2<T> Line<T>::closest_point(const Vec2<T>& pt) const {
		auto v = b - a;
		auto w = pt - a;
		auto t = Vec2<T>::dot(w, v) / (v.x * v.x + v.y * v.y);
		if (t < 0) t = 0;
		else if (t > 1) t = 1;
		return v * t + a;
	}

	template<class T>
	constexpr bool Line<T>::intersects(const Rect<T>& rect, Vec2<T>* out_intersection_point) const
	{
		u8 ca = rect.get_sector(a);
		u8 cb = rect.get_sector(b);

		if (ca == cb || (ca & cb) != 0)
			return false;

		u8 both = ca | cb;

		// top
		if ((both & 0b0100) != 0 && intersects(rect.top_line(), out_intersection_point))
			return true;

		// bottom
		if ((both & 0b1000) != 0 && intersects(rect.bottom_line(), out_intersection_point))
			return true;

		// left
		if ((both & 0b0001) != 0 && intersects(rect.left_line(), out_intersection_point))
			return true;

		// right
		if ((both & 0b0010) != 0 && intersects(rect.right_line(), out_intersection_point))
			return true;

		return false;
	}

	template<class T>
	constexpr bool Line<T>::intersects(const Line<T>& line, Vec2<T>* out_intersection_point) const
	{
		Vec2<T> e = b - a;
		Vec2<T> d = line.b - line.a;
		T e_dot_d_perp = e.x * d.y - e.y * d.x;

		// if e dot d == 0, it means the lines are parallel
		// so have infinite intersection points
		if (e_dot_d_perp > -epsilon && e_dot_d_perp < epsilon)
			return false;

		Vec2<T> c = line.a - a;
		T t = (c.x * d.y - c.y * d.x) / e_dot_d_perp;
		if (t < 0 || t > 1)
			return false;

		T u = (c.x * e.y - c.y * e.x) / e_dot_d_perp;
		if (u < 0 || u > 1)
			return false;

		if (out_intersection_point)
			*out_intersection_point = (e * t) + a;

		return true;
	}

	template<class T>
	constexpr void Line<T>::project(const Vec2<T>& axis, T* min, T* max) const {
		T dot = a.x * axis.x + a.y * axis.y;
		*min = dot;
		*max = dot;
		dot = b.x * axis.x + b.y * axis.y;
		*min = dot < *min ? dot : *min;
		*max = dot > *max ? dot : *max;
	}

	template<class T>
	constexpr Line<T> Line<T>::operator+(const Vec2<T>& rhs) const {
		return Line(a + rhs, b + rhs);
	}

	template<class T>
	constexpr Line<T> Line<T>::operator-(const Vec2<T>& rhs) const {
		return Line(a - rhs, b - rhs);
	}

	template<class T>
	constexpr Mat3x2<T>::Mat3x2()
		: m11(0), m12(0), m21(0), m22(0), m31(0), m32(0) {}

	template<class T>
	constexpr Mat3x2<T>::Mat3x2(T m11, T m12, T m21, T m22, T m31, T m32)
		: m11(m11), m12(m12), m21(m21), m22(m22), m31(m31), m32(m32) {}

	template<class T>
	template<class K>
	constexpr Mat3x2<T>::Mat3x2(const Mat3x2<K>& m)
		: m11(static_cast<T>(m.m11)), m12(static_cast<T>(m.m12))
		, m21(static_cast<T>(m.m21)), m22(static_cast<T>(m.m22))
		, m31(static_cast<T>(m.m31)), m32(static_cast<T>(m.m32)) {}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::invert() const {
		auto det = (m11 * m22) - (m21 * m12);
		auto inv_det = 1 / det;

		return Mat3x2<T>
		(
			m22 * inv_det,
			-m12 * inv_det,
			-m21 * inv_det,
			m11 * inv_det,
			(m21 * m32 - m31 * m22) * inv_det,
			(m31 * m12 - m11 * m32) * inv_det
		);
	}

	template<class T>
	T Mat3x2<T>::scaling_factor() const {
		return Calc::sqrt(m11 * m11 + m12 * m12);
	}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::create_translation(const Vec2<T>& position) {
		return Mat3x2(1, 0, 0, 1, position.x, position.y);
	}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::create_translation(T x, T y) {
		return Mat3x2(1, 0, 0, 1, x, y);
	}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::create_scale(T scale) {
		return Mat3x2(scale, 0, 0, scale, 0, 0);
	}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::create_scale(T x, T y) {
		return Mat3x2(x, 0, 0, y, 0, 0);
	}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::create_scale(const Vec2<T>& scale) {
		return Mat3x2(scale.x, 0, 0, scale.y, 0, 0);
	}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::create_scale(T scale, const Vec2<T>& center_point) {
		auto tx = center_point.x * (1 - scale);
		auto ty = center_point.y * (1 - scale);

		Mat3x2<T> result;
		result.m11 = scale;
		result.m12 = 0;
		result.m21 = 0;
		result.m22 = scale;
		result.m31 = tx;
		result.m32 = ty;
		return result;
	}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::create_scale(const Vec2<T>& scale, const Vec2<T>& center_point) {
		auto tx = center_point.x * (1 - scale.x);
		auto ty = center_point.y * (1 - scale.y);

		Mat3x2<T> result;
		result.m11 = scale.x;
		result.m12 = 0;
		result.m21 = 0;
		result.m22 = scale.y;
		result.m31 = tx;
		result.m32 = ty;
		return result;
	}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::create_scale(T scale_x, T scale_y, const Vec2<T>& center_point) {
		auto tx = center_point.x * (1 - scale_x);
		auto ty = center_point.y * (1 - scale_y);

		Mat3x2<T> result;
		result.m11 = scale_x;
		result.m12 = 0;
		result.m21 = 0;
		result.m22 = scale_y;
		result.m31 = tx;
		result.m32 = ty;
		return result;
	}

	template<class T>
	Mat3x2<T> Mat3x2<T>::create_rotation(T radians) {
		auto c = Calc::cos(radians);
		auto s = Calc::sin(radians);
		return Mat3x2<T>(c, s, -s, c, 0, 0);
	}

	template<class T>
	Mat3x2<T> Mat3x2<T>::create_transform(const Vec2<T>& position, const Vec2<T>& origin, const Vec2<T>& scale, T rotation) {
		Mat3x2<T> matrix = identity;
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

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::add(const Mat3x2& a, const Mat3x2& b) {
		return Mat3x2(
			a.m11 + b.m11,
			a.m12 + b.m12,
			a.m21 + b.m21,
			a.m22 + b.m22,
			a.m31 + b.m31,
			a.m32 + b.m32);
	}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::subtract(const Mat3x2& a, const Mat3x2& b) {
		return Mat3x2(
			a.m11 - b.m11,
			a.m12 - b.m12,
			a.m21 - b.m21,
			a.m22 - b.m22,
			a.m31 - b.m31,
			a.m32 - b.m32);
	}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::multiply(const Mat3x2& a, const Mat3x2& b) {
		return Mat3x2(a.m11 * b.m11 + a.m12 * b.m21,
			a.m11 * b.m12 + a.m12 * b.m22,
			a.m21 * b.m11 + a.m22 * b.m21,
			a.m21 * b.m12 + a.m22 * b.m22,
			a.m31 * b.m11 + a.m32 * b.m21 + b.m31,
			a.m31 * b.m12 + a.m32 * b.m22 + b.m32);
	}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::operator *(const Mat3x2& rhs) const {
		return multiply(*this, rhs);
	}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::operator +(const Mat3x2& rhs) const {
		return add(*this, rhs);
	}

	template<class T>
	constexpr Mat3x2<T> Mat3x2<T>::operator -(const Mat3x2& rhs) const {
		return subtract(*this, rhs);
	}

	template<class T>
	constexpr Mat3x2<T>& Mat3x2<T>::operator *=(const Mat3x2& rhs) {
		*this = multiply(*this, rhs);
		return *this;
	}

	template<class T>
	constexpr bool Mat3x2<T>::operator==(const Mat3x2& rhs) {
		return 
			Calc::abs(m11 - rhs.m11) < epsilon &&
			Calc::abs(m12 - rhs.m12) < epsilon &&
			Calc::abs(m21 - rhs.m21) < epsilon &&
			Calc::abs(m22 - rhs.m22) < epsilon &&
			Calc::abs(m31 - rhs.m31) < epsilon &&
			Calc::abs(m32 - rhs.m32) < epsilon;
	}

	template<class T>
	constexpr bool Mat3x2<T>::operator!=(const Mat3x2& rhs) {
		return !(*this == rhs);
	}

	template<class T>
	Mat4x4<T>::Mat4x4() :
		m11(0.0f), m12(0.0f), m13(0.0f), m14(0.0f),
		m21(0.0f), m22(0.0f), m23(0.0f), m24(0.0f),
		m31(0.0f), m32(0.0f), m33(0.0f), m34(0.0f),
		m41(0.0f), m42(0.0f), m43(0.0f), m44(0.0f) {}

	template<class T>
	Mat4x4<T>::Mat4x4(
		T m11, T m12, T m13, T m14,
		T m21, T m22, T m23, T m24,
		T m31, T m32, T m33, T m34,
		T m41, T m42, T m43, T m44) :
		m11(m11), m12(m12), m13(m13), m14(m14),
		m21(m21), m22(m22), m23(m23), m24(m24),
		m31(m31), m32(m32), m33(m33), m34(m34),
		m41(m41), m42(m42), m43(m43), m44(m44) {}

	template<class T>
	constexpr Mat4x4<T> Mat4x4<T>::create_ortho(T width, T height, T z_near_plane, T z_far_plane) {
		Mat4x4<T> result = identity;
		result.m11 = 2 / width;
		result.m12 = result.m13 = result.m14 = 0;
		result.m22 = -2 / height;
		result.m21 = result.m23 = result.m24 = 0;
		result.m33 = 1 / (z_near_plane - z_far_plane);
		result.m31 = result.m32 = result.m34 = 0;
		result.m41 = result.m42 = 0;
		result.m43 = z_near_plane / (z_near_plane - z_far_plane);
		result.m44 = 1;
		return result;
	}

	template<class T>
	constexpr Mat4x4<T> Mat4x4<T>::create_ortho_offcenter(T left, T right, T bottom, T top, T z_near_plane, T z_far_plane) {
		Mat4x4<T> result = identity;
		result.m11 = 2 / (right - left);
		result.m12 = result.m13 = result.m14 = 0;
		result.m22 = 2 / (top - bottom);
		result.m21 = result.m23 = result.m24 = 0;
		result.m33 = 1 / (z_near_plane - z_far_plane);
		result.m31 = result.m32 = result.m34 = 0;
		result.m41 = (left + right) / (left - right);
		result.m42 = (top + bottom) / (bottom - top);
		result.m43 = z_near_plane / (z_near_plane - z_far_plane);
		result.m44 = 1;
		return result;
	}

	template<class T>
	Mat4x4<T> Mat4x4<T>::create_perspective(T field_of_view, T ratio, T z_near_plane, T z_far_plane) {
		auto scale_x = 1 / Calc::tan(field_of_view * 0.5);
		auto scale_y = scale_x / ratio;

		Mat4x4 result;
		result.m11 = scale_y;
		result.m12 = result.m13 = result.m14 = 0;
		result.m22 = scale_x;
		result.m21 = result.m23 = result.m24 = 0;
		result.m31 = result.m32 = 0;
		result.m33 = z_far_plane / (z_near_plane - z_far_plane);
		result.m34 = -1;
		result.m41 = result.m42 = result.m44 = 0;
		result.m43 = z_near_plane * z_far_plane / (z_near_plane - z_far_plane);
		return result;
	}

	template<class T>
	constexpr Mat4x4<T> Mat4x4<T>::create_translation(T x, T y, T z) {
		Mat4x4 result = identity;
		result.m41 = x;
		result.m42 = y;
		result.m43 = z;
		return result;
	}

	template<class T>
	constexpr Mat4x4<T> Mat4x4<T>::create_scale(T x, T y, T z) {
		Mat4x4 result = identity;
		result.m11 = x;
		result.m22 = y;
		result.m33 = z;
		return result;
	}

	template<class T>
	Mat4x4<T> Mat4x4<T>::create_lookat(const Vec3<T>& position, const Vec3<T>& target, const Vec3<T>& up) {
		auto zaxis = (position - target).normal();
		auto xaxis = Vec3<T>::cross(up, zaxis).normal();
		auto yaxis = Vec3<T>::cross(zaxis, xaxis);

		Mat4x4 result;
		result.m11 = xaxis.x;
		result.m12 = yaxis.x;
		result.m13 = zaxis.x;
		result.m14 = 0.0f;
		result.m21 = xaxis.y;
		result.m22 = yaxis.y;
		result.m23 = zaxis.y;
		result.m24 = 0.0f;
		result.m31 = xaxis.z;
		result.m32 = yaxis.z;
		result.m33 = zaxis.z;
		result.m34 = 0.0f;
		result.m41 = -Vec3<T>::dot(xaxis, position);
		result.m42 = -Vec3<T>::dot(yaxis, position);
		result.m43 = -Vec3<T>::dot(zaxis, position);
		result.m44 = 1.0f;
		return result;
	}

	template<class T>
	constexpr Mat4x4<T> Mat4x4<T>::operator*(const Mat4x4& rhs) {
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
}
