#pragma once
#include <blah/math/color.h>
#include <blah/math/rectI.h>
#include <blah/math/point.h>

namespace Blah
{
	class Stream;

	struct Image
	{
		int width = 0;
		int height = 0;
		Color* pixels = nullptr;

		Image();
		Image(Stream& stream);
		Image(const char* file);
		Image(int width, int height);
		Image(const Image& src);
		Image& operator=(const Image& src);
		Image(Image&& src) noexcept;
		Image& operator=(Image&& src) noexcept;
		~Image();

		void from_stream(Stream& stream);
		void dispose();

		void premultiply();
		void set_pixels(const RectI& rect, Color* data);
		bool save_png(const char* file) const;
		bool save_png(Stream& stream) const;
		bool save_jpg(const char* file, int quality) const;
		bool save_jpg(Stream& stream, int quality) const;
		void get_pixels(Color* dest, const Point& destPos, const Point& destSize, RectI sourceRect);
		Image get_sub_image(const RectI& sourceRect);
	};
}