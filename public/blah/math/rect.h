#pragma once
#include <blah/math/point.h>
#include <blah/math/vec2.h>
#include <blah/math/rectI.h>
#include <blah/math/line.h>

namespace Blah
{
	struct Mat3x2;

	struct Rect
	{
		float x;
		float y;
		float w;
		float h;

		Rect();
		Rect(float rx, float ry, float rw, float rh);
		Rect(Vec2 pos, Vec2 size);
		Rect(RectI r);

		Rect scale(float s);
		Rect scale(float sx, float sy);

		float left() const { return x; }
		float right() const { return x + w; }
		float top() const { return y; }
		float bottom() const { return y + h; }

		Vec2 center() const { return Vec2(x + w / 2, y + h / 2); }
		float center_x() const { return x + w / 2; }
		float center_y() const { return y + h / 2; }

		Vec2 top_left() const { return Vec2(x, y); }
		Vec2 top_right() const { return Vec2(x + w, y); }
		Vec2 bottom_right() const { return Vec2(x + w, y + h); }
		Vec2 bottom_left() const { return Vec2(x, y + h); }

		Vec2 center_left() const { return Vec2(x, y + h / 2); }
		Vec2 center_right() const { return Vec2(x + w, y + h / 2); }
		Vec2 middle_top() const { return Vec2(x + w / 2, y); }
		Vec2 middle_bottom() const { return Vec2(x + w / 2, y + h); }

		Line left_line() const;
		Line right_line() const;
		Line top_line() const;
		Line bottom_line() const;

		bool contains(const Point& pt) const { return pt.x >= x && pt.x < x + w && pt.y >= y && pt.y < y + h; }
		bool contains(const Vec2& pt) const { return pt.x >= x && pt.x < x + w && pt.y >= y && pt.y < y + h; }

		bool overlaps(const Rect& rect) const { return x + w >= rect.x && y + h >= rect.y && x < rect.x + rect.w && y < rect.y + rect.h; }

		bool intersects(const Line& line) const { return line.intersects(*this); }
		bool intersects(const Line& line, Vec2* out_intersection_point) const { return line.intersects(*this, out_intersection_point); }
		bool intersects(const Vec2& line_from, const Vec2& line_to) const { return intersects(Line(line_from, line_to)); }
		bool intersects(const Vec2& line_from, const Vec2& line_to, Vec2* out_intersection_point) const { return intersects(Line(line_from, line_to), out_intersection_point); }

		Vec2 intersection_point(const Line& line) const
		{
			Vec2 ret;
			if (line.intersects(*this, &ret))
				return ret;
			else
				return Vec2::zero;
		}

		Vec2 intersection_point(const Vec2& line_from, const Vec2& line_to) const
		{
			Vec2 ret;
			if (Line(line_from, line_to).intersects(*this, &ret))
				return ret;
			else
				return Vec2::zero;
		}

		Rect inflate(float amount) { return Rect(x - amount, y - amount, w + amount * 2, h + amount * 2); }

		/*
			Rect Sectors:
				0101  0100  0110
				0001  0000  0010
				1001  1000  1010
			0000 = inside rectangle, all others refer to sectors relative to the rectangle
		*/
		char get_sector(const Vec2& pt) const;

		bool operator==(const Rect& rhs) const { return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h; }
		bool operator!=(const Rect& rhs) const { return !(*this == rhs); }

		Rect operator+(const Vec2& rhs) const;
		Rect operator-(const Vec2& rhs) const;
		Rect& operator+=(const Vec2& rhs);
		Rect& operator-=(const Vec2& rhs);

		static Rect transform(const Rect& vec, const Mat3x2& matrix);
		static Rect transform(float x, float y, float w, float h, const Mat3x2& matrix);
		static Rect from_points(Vec2& from, Vec2& to);
	};
}
