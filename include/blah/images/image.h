#pragma once
#include <blah/numerics/color.h>
#include <blah/numerics/rectI.h>
#include <blah/numerics/point.h>
#include <blah/filesystem.h>

namespace Blah
{
	class Stream;

	// A 2D Bitmap stored on the CPU.
	// For drawing images to the screen, use a Texture.
	class Image
	{
	public:

		// width of the image, in pixels.
		int width = 0;

		// height of the image, in pixels.
		int height = 0;

		// pixel data of the image.
		// this can be nullptr if the image is never assigned to anything.
		Color* pixels = nullptr;

		Image();
		Image(Stream& stream);
		Image(const FilePath& file);
		Image(int width, int height);
		Image(const Image& src);
		Image& operator=(const Image& src);
		Image(Image&& src) noexcept;
		Image& operator=(Image&& src) noexcept;
		~Image();

		// disposes the existing image and recreates it from a stream
		void from_stream(Stream& stream);

		// disposes the image and resets its values to defaults
		void dispose();

		// applies alpha premultiplication to the image data
		void premultiply();

		// sets the pixels at the provided rectangle to the given data
		// data must be at least rect.w * rect.h in size!
		void set_pixels(const RectI& rect, Color* data);

		// saves the image to a png file
		bool save_png(const FilePath& file) const;

		// saves the image to a png file
		bool save_png(Stream& stream) const;

		// saves the image to a jpg file
		bool save_jpg(const FilePath& file, int quality) const;

		// saves the image to a jpg file
		bool save_jpg(Stream& stream, int quality) const;

		// gets the pixels from the given source rectangle
		void get_pixels(Color* dest, const Point& dest_pos, const Point& dest_size, RectI source_rect) const;

		// gets a sub image from this image
		Image get_sub_image(const RectI& source_rect);

	private:

		// whether the stbi library owns the image data.
		// we should let it free the data if it created it.
		bool m_stbi_ownership;
	};
}