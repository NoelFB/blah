#include <blah_image.h>

using namespace Blah;

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include "third_party/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party/stb_image_write.h"

namespace
{
	int blah_stbi_read(void* user, char* data, int size)
	{
		i64 read = ((Stream*)user)->read(data, size);
		return (int)read;
	}

	void blah_stbi_skip(void* user, int n)
	{
		((Stream*)user)->seek(((Stream*)user)->position() + n);
	}

	int blaH_stbi_eof(void* user)
	{
		i64 position = ((Stream*)user)->position();
		i64 length = ((Stream*)user)->length();

		if (position >= length)
			return 1;

		return 0;
	}

	void blah_stbi_write(void* context, void* data, int size)
	{
		((Stream*)context)->write((char*)data, size);
	}
}

Image::Image()
{
	width = height = 0;
	pixels = nullptr;
	m_stbi_ownership = false;
}

Image::Image(Stream& stream)
{
	width = height = 0;
	pixels = nullptr;
	m_stbi_ownership = false;
	from_stream(stream);
}

Image::Image(const FilePath& file)
{
	width = height = 0;
	pixels = nullptr;
	m_stbi_ownership = false;

	FileStream fs(file, FileMode::OpenRead);
	if (fs.is_readable())
		from_stream(fs);
}

Image::Image(int w, int h)
{
	BLAH_ASSERT(w >= 0 && h >= 0, "Image width and height must be larger than 0");

	width = w;
	height = h;
	pixels = new Color[width * height];
	m_stbi_ownership = false;
	memset(pixels, 0, (size_t)width * (size_t)height * sizeof(Color));
}

Image::Image(const Image& src)
{
	width = src.width;
	height = src.height;
	m_stbi_ownership = src.m_stbi_ownership;
	pixels = nullptr;

	if (src.pixels != nullptr && width > 0 && height > 0)
	{
		pixels = new Color[width * height];
		memcpy(pixels, src.pixels, sizeof(Color) * width * height);
	}
}

Image& Image::operator=(const Image& src)
{
    dispose();

	width = src.width;
	height = src.height;
	m_stbi_ownership = src.m_stbi_ownership;
	pixels = nullptr;

	if (src.pixels != nullptr && width > 0 && height > 0)
	{
		pixels = new Color[width * height];
		memcpy(pixels, src.pixels, sizeof(Color) * width * height);
	}

	return *this;
}

Image::Image(Image&& src) noexcept
{
	width = src.width;
	height = src.height;
	pixels = src.pixels;
	m_stbi_ownership = src.m_stbi_ownership;
	src.width = src.height = 0;
	src.pixels = nullptr;
	src.m_stbi_ownership = false;
}

Image& Image::operator=(Image&& src) noexcept
{
	dispose();

	width = src.width;
	height = src.height;
	pixels = src.pixels;
	m_stbi_ownership = src.m_stbi_ownership;
	src.width = src.height = 0;
	src.pixels = nullptr;
	src.m_stbi_ownership = false;
	return *this;
}

Image::~Image()
{
	dispose();
}

bool Image::from_stream(Stream& stream)
{
	dispose();

	if (!stream.is_readable())
		return false;

	stbi_io_callbacks callbacks;
	callbacks.eof = blaH_stbi_eof;
	callbacks.read = blah_stbi_read;
	callbacks.skip = blah_stbi_skip;

	int x, y, comps;
	u8* data = stbi_load_from_callbacks(&callbacks, &stream, &x, &y, &comps, 4);

	if (data == nullptr)
		return false;

	m_stbi_ownership = true;
	pixels = (Color*)data;
	width = x;
	height = y;

	return true;
}

void Image::dispose()
{
	if (m_stbi_ownership)
		stbi_image_free(pixels);
	else
		delete[] pixels;

	pixels = nullptr;
	width = height = 0;
	m_stbi_ownership = false;
}

void Image::premultiply()
{
	if (pixels != nullptr)
	{
		for (int n = 0; n < width * height; n ++)
		{
			pixels[n].r = (u8)(pixels[n].r * pixels[n].a / 255);
			pixels[n].g = (u8)(pixels[n].g * pixels[n].a / 255);
			pixels[n].b = (u8)(pixels[n].b * pixels[n].a / 255);
		}
	}
}

void Image::set_pixels(const Recti& rect, Color* data)
{
	for (int y = 0; y < rect.h; y++)
	{
		int to = rect.x + ((rect.y + y) * width);
		int from = (y * rect.w);
		memcpy(pixels + to, data + from, sizeof(Color) * rect.w);
	}
}

bool Image::save_png(const FilePath& file) const
{
	FileStream fs(file, FileMode::CreateWrite);
	return save_png(fs);
}

bool Image::save_png(Stream& stream) const
{
	BLAH_ASSERT(pixels != nullptr, "Image Pixel data cannot be null");
	BLAH_ASSERT(width > 0 && height > 0, "Image Width and Height must be larger than 0");
	
	if (stream.is_writable())
	{
		stbi_write_force_png_filter = 0;
		stbi_write_png_compression_level = 0;

		if (stbi_write_png_to_func(blah_stbi_write, &stream, width, height, 4, pixels, width * 4) != 0)
			return true;
	}

	return false;
}

bool Image::save_jpg(const FilePath& file, int quality) const
{
	FileStream fs(file, FileMode::CreateWrite);
	return save_jpg(fs, quality);
}

bool Image::save_jpg(Stream& stream, int quality) const
{
	BLAH_ASSERT(pixels != nullptr, "Image Pixel data cannot be null");
	BLAH_ASSERT(width > 0 && height > 0, "Image Width and Height must be larger than 0");

	if (quality < 1)
	{
		Log::warn("jpg quality value should be between 1 and 100; input was %i", quality);
		quality = 1;
	}
	else if (quality > 100)
	{
		Log::warn("jpg quality value should be between 1 and 100; input was %i", quality);
		quality = 100;
	}

	if (stream.is_writable())
	{
		if (stbi_write_jpg_to_func(blah_stbi_write, &stream, width, height, 4, pixels, quality) != 0)
			return true;
	}

	return false;
}

void Image::get_pixels(Color* dest, const Point& dest_pos, const Point& dest_size, Recti source_rect) const
{
	// can't be outside of the source image
	if (source_rect.x < 0) source_rect.x = 0;
	if (source_rect.y < 0) source_rect.y = 0;
	if (source_rect.x + source_rect.w > width) source_rect.w = width - source_rect.x;
	if (source_rect.y + source_rect.h > height) source_rect.h = height - source_rect.y;

	// can't be larger than our destination
	if (source_rect.w > dest_size.x - dest_pos.x)
		source_rect.w = dest_size.x - dest_pos.x;
	if (source_rect.h > dest_size.y - dest_pos.y)
		source_rect.h = dest_size.y - dest_pos.y;

	for (int y = 0; y < source_rect.h; y++)
	{
		int to = dest_pos.x + (dest_pos.y + y) * dest_size.x;
		int from = source_rect.x + (source_rect.y + y) * width;
		memcpy(dest + to, pixels + from, sizeof(Color) * (int)source_rect.w);
	}
}

Image Image::get_sub_image(const Recti& source_rect)
{
	Image img(source_rect.w, source_rect.h);
	get_pixels(img.pixels, Point::zero, Point(img.width, img.height), source_rect);
	return img;
}
