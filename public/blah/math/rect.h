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

		float left() const;
		float right() const;
		float top() const;
		float bottom() const;

		Vec2 center() const;
		float center_x() const;
		float center_y() const;

		Vec2 top_left() const;
		Vec2 top_right() const;
		Vec2 bottom_right() const;
		Vec2 bottom_left() const;

		Vec2 center_left() const;
		Vec2 center_right() const;
		Vec2 middle_top() const;
		Vec2 middle_bottom() const;

		Line left_line() const;
		Line right_line() const;
		Line top_line() const;
		Line bottom_line() const;

		bool contains(const Point& pt) const;
		bool contains(const Vec2& pt) const;

		bool overlaps(const Rect& rect) const { return x + w >= rect.x && y + h >= rect.y && x < rect.x + rect.w && y < rect.y + rect.h; }
		Rect overlap_rect(const Rect& other) const;

		bool intersects(const Line& line) const;
		bool intersects(const Line& line, Vec2* out_intersection_point) const;
		bool intersects(const Vec2& line_from, const Vec2& line_to) const;
		bool intersects(const Vec2& line_from, const Vec2& line_to, Vec2* out_intersection_point) const;

		Vec2 intersection_point(const Line& line) const;
		Vec2 intersection_point(const Vec2& line_from, const Vec2& line_to) const;

		Rect inflate(float amount) const;

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
