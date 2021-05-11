#pragma once
#include <blah/numerics/point.h>
#include <blah/numerics/vec2.h>
#include <blah/numerics/rectI.h>
#include <blah/numerics/line.h>
#include <blah/numerics/mat3x2.h>

namespace Blah
{
	struct Rect
	{
		float x;
		float y;
		float w;
		float h;

		constexpr Rect()
			: x(0), y(0), w(0), h(0) {}

		constexpr Rect(float x, float y, float w, float h)
			: x(x), y(y), w(w), h(h) {}

		constexpr Rect(Vec2 pos, Vec2 size)
			: x(pos.x), y(pos.y), w(size.x), h(size.y) {}

		constexpr Rect(const RectI& r)
			: x(static_cast<float>(r.x)), y(static_cast<float>(r.y)), w(static_cast<float>(r.w)), h(static_cast<float>(r.h)) {}

		constexpr float left() const         { return x; }
		constexpr float right() const        { return x + w; }
		constexpr float top() const          { return y; }
		constexpr float bottom() const       { return y + h; }

		constexpr Vec2 center() const        { return Vec2(x + w / 2, y + h / 2); }
		constexpr float center_x() const     { return x + w / 2; }
		constexpr float center_y() const     { return y + h / 2; }

		constexpr Vec2 top_left() const      { return Vec2(x, y); }
		constexpr Vec2 top_right() const     { return Vec2(x + w, y); }
		constexpr Vec2 bottom_right() const  { return Vec2(x + w, y + h); }
		constexpr Vec2 bottom_left() const   { return Vec2(x, y + h); }

		constexpr Vec2 center_left() const   { return Vec2(x, y + h / 2); }
		constexpr Vec2 center_right() const  { return Vec2(x + w, y + h / 2); }
		constexpr Vec2 middle_top() const    { return Vec2(x + w / 2, y); }
		constexpr Vec2 middle_bottom() const { return Vec2(x + w / 2, y + h); }

		constexpr Line left_line() const     { return Line(left(), top(), left(), bottom()); }
		constexpr Line right_line() const    { return Line(right(), top(), right(), bottom()); }
		constexpr Line top_line() const      { return Line(left(), top(), right(), top()); }
		constexpr Line bottom_line() const   { return Line(left(), bottom(), right(), bottom()); }

		constexpr bool contains(const Point& pt) const
		{
			return pt.x >= x && pt.x < x + w && pt.y >= y && pt.y < y + h;
		}

		constexpr bool contains(const Vec2& pt) const
		{
			return pt.x >= x && pt.x < x + w && pt.y >= y && pt.y < y + h;
		}

		constexpr bool overlaps(const Rect& rect) const
		{
			return x + w >= rect.x && y + h >= rect.y && x < rect.x + rect.w && y < rect.y + rect.h;
		}

		Rect overlap_rect(const Rect& other) const;

		bool intersects(const Line& line) const;
		bool intersects(const Line& line, Vec2* out_intersection_point) const;
		bool intersects(const Vec2& line_from, const Vec2& line_to) const;
		bool intersects(const Vec2& line_from, const Vec2& line_to, Vec2* out_intersection_point) const;

		Vec2 intersection_point(const Line& line) const;
		Vec2 intersection_point(const Vec2& line_from, const Vec2& line_to) const;

		constexpr Rect scale(float s) const
		{
			return Rect(x * s, y * s, w * s, h * s);
		}

		constexpr Rect scale(float sx, float sy) const
		{
			return Rect(x * sx, y * sy, w * sx, h * sy);
		}

		constexpr Rect inflate(float amount) const
		{
			return Rect(x - amount, y - amount, w + amount * 2, h + amount * 2);
		}

		// Rect Sectors:
		//		0101  0100  0110
		//		0001  0000  0010
		//		1001  1000  1010
		//	0000 = inside rectangle, all others refer to sectors relative to the rectangle
		constexpr char get_sector(const Vec2& pt) const
		{
			char h = 0;
			if (pt.x < left())
				h = 0b0001;
			else if (pt.x >= right())
				h = 0b0010;

			char v = 0;
			if (pt.y < top())
				v = 0b0100;
			else if (pt.y >= bottom())
				v = 0b1000;

			return h | v;
		}

		constexpr Rect operator+(const Vec2& rhs) const
		{
			return Rect(x + rhs.x, y + rhs.y, w, h);
		}
		
		constexpr Rect operator-(const Vec2& rhs) const
		{
			return Rect(x - rhs.x, y - rhs.y, w, h);
		}
		
		constexpr Rect& operator+=(const Vec2& rhs)
		{
			x += rhs.x; y += rhs.y;
			return *this;
		}
		
		constexpr Rect& operator-=(const Vec2& rhs)
		{
			x -= rhs.x; y -= rhs.y;
			return *this;
		}

		constexpr bool operator==(const Rect& rhs) const
		{
			return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h;
		}

		constexpr bool operator!=(const Rect& rhs) const
		{
			return x != rhs.x || y != rhs.y || w != rhs.w || h != rhs.h;
		}

		static constexpr Rect transform(const Rect& rect, const Mat3x2& matrix)
		{
			return Rect(
				(rect.x * matrix.m11) + (rect.y * matrix.m21) + matrix.m31,
				(rect.x * matrix.m12) + (rect.y * matrix.m22) + matrix.m32,
				(rect.w * matrix.m11) + (rect.h * matrix.m21),
				(rect.w * matrix.m12) + (rect.h * matrix.m22));
		}

		static constexpr Rect transform(float x, float y, float w, float h, const Mat3x2& matrix)
		{
			return Rect(
				(x * matrix.m11) + (y * matrix.m21) + matrix.m31,
				(x * matrix.m12) + (y * matrix.m22) + matrix.m32,
				(w * matrix.m11) + (h * matrix.m21),
				(w * matrix.m12) + (h * matrix.m22));
		}

		static constexpr Rect from_points(Vec2& from, Vec2& to)
		{
			auto min = Vec2(from.x < to.x ? from.x : to.x, from.y < to.y ? from.y : to.y);
			auto max = Vec2(from.x > to.x ? from.x : to.x, from.y > to.y ? from.y : to.y);

			return Rect(min.x, min.y, max.x - min.x, max.y - min.y);
		}
	};
}
