#pragma once

namespace Blah
{
	struct Point;
	struct Rect;
	struct Vec2;

	struct RectI
	{
		int x;
		int y;
		int w;
		int h;

		RectI();
		RectI(int rx, int ry, int rw, int rh);
		RectI(Point pos, Point size);

		int left() const;
		int right() const;
		int top() const;
		int bottom() const;
		int center_x() const;
		int center_y() const;

		Point center() const;
		Point top_left() const;
		Point top_right() const;
		Point bottom_left() const;
		Point bottom_right() const;

		bool overlaps(const RectI& other) const;
		bool contains(const Point& pt) const;
		bool contains(const Vec2& pt) const;

		/*
			Rect Sectors:
				0101  0100  0110
				0001  0000  0010
				1001  1000  1010
			0000 = inside rectangle, all others refer to sectors relative to the rectangle
		*/
		char get_sector(const Point& pt) const;
		char get_sector(const Vec2& pt) const;

		bool operator==(const RectI& rhs) const;
		bool operator!=(const RectI& rhs) const;

		RectI operator+(const Point& rhs) const;
		RectI operator-(const Point& rhs) const;
		RectI operator*(const int& rhs) const;
		RectI operator/(const int& rhs) const;
		RectI& operator+=(const Point& rhs);
		RectI& operator-=(const Point& rhs);
	};
}