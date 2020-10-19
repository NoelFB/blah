#include <blah/images/image.h>
#include <blah/streams/stream.h>
#include <blah/streams/filestream.h>
#include <blah/log.h>

using namespace Blah;

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#include <blah/third_party/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <blah/third_party/stb_image_write.h>

namespace
{
	int Blah_STBI_Read(void* user, char* data, int size)
	{
		int64_t read = ((Stream*)user)->read(data, size);
		return (int)read;
	}

	void Blah_STBI_Skip(void* user, int n)
	{
		((Stream*)user)->seek(((Stream*)user)->position() + n);
	}

	int Blah_STBI_Eof(void* user)
	{
		int64_t position = ((Stream*)user)->position();
		int64_t length = ((Stream*)user)->length();

		if (position >= length)
			return 1;

		return 0;
	}

	void Blah_STBI_Write(void* context, void* data, int size)
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

Image::Image(const char* file)
{
	width = height = 0;
	pixels = nullptr;
	m_stbi_ownership = false;

	FileStream fs(file, FileMode::Read);
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

void Image::from_stream(Stream& stream)
{
	dispose();

	if (!stream.is_readable())
	{
		BLAH_ERROR("Unable to load image as the Stream was not readable");
		return;
	}

	stbi_io_callbacks callbacks;
	callbacks.eof = Blah_STBI_Eof;
	callbacks.read = Blah_STBI_Read;
	callbacks.skip = Blah_STBI_Skip;

	int x, y, comps;
	uint8_t* data = stbi_load_from_callbacks(&callbacks, &stream, &x, &y, &comps, 4);

	if (data == nullptr)
	{
		BLAH_ERROR("Unable to load image as the Stream's data was not a valid image");
		return;
	}

	m_stbi_ownership = true;
	pixels = (Color*)data;
	width = x;
	height = y;
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
			pixels[n].r = (uint8_t)(pixels[n].r * pixels[n].a / 255);
			pixels[n].g = (uint8_t)(pixels[n].g * pixels[n].a / 255);
			pixels[n].b = (uint8_t)(pixels[n].b * pixels[n].a / 255);
		}
	}
}

void Image::set_pixels(const RectI& rect, Color* data)
{
	for (int y = 0; y < rect.h; y++)
	{
		int to = rect.x + ((rect.y + y) * width);
		int from = (y * rect.w);
		memcpy(pixels + to, data + from, sizeof(Color) * rect.w);
	}
}

bool Image::save_png(const char* file) const
{
	FileStream fs(file, FileMode::Write);
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

		if (stbi_write_png_to_func(Blah_STBI_Write, &stream, width, height, 4, pixels, width * 4) != 0)
			return true;
		else
			Log::error("stbi_write_png_to_func failed");
	}
	else
	{
		Log::error("Cannot save Image, the Stream is not writable");
	}

	return false;
}

bool Image::save_jpg(const char* file, int quality) const
{
	FileStream fs(file, FileMode::Write);
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
		if (stbi_write_jpg_to_func(Blah_STBI_Write, &stream, width, height, 4, pixels, quality) != 0)
			return true;
		else
			Log::error("stbi_write_jpg_to_func failed");
	}
	else
	{
		Log::error("Cannot save Image, the Stream is not writable");
	}

	return false;
}



void Image::get_pixels(Color* dest, const Point& destPos, const Point& destSize, RectI sourceRect)
{
	// can't be outside of the source image
	if (sourceRect.x < 0) sourceRect.x = 0;
	if (sourceRect.y < 0) sourceRect.y = 0;
	if (sourceRect.x + sourceRect.w > width) sourceRect.w = width - sourceRect.x;
	if (sourceRect.y + sourceRect.h > height) sourceRect.h = height - sourceRect.y;

	// can't be larger than our destination
	if (sourceRect.w > destSize.x - destPos.x)
		sourceRect.w = destSize.x - destPos.x;
	if (sourceRect.h > destSize.y - destPos.y)
		sourceRect.h = destSize.y - destPos.y;

	for (int y = 0; y < sourceRect.h; y++)
	{
		int to = destPos.x + (destPos.y + y) * destSize.x;
		int from = sourceRect.x + (sourceRect.y + y) * width;
		memcpy(dest + to, pixels + from, sizeof(Color) * (int)sourceRect.w);
	}
}

Image Image::get_sub_image(const RectI& sourceRect)
{
	Image img(sourceRect.w, sourceRect.h);
	get_pixels(img.pixels, Point::zero, Point(img.width, img.height), sourceRect);
	return img;
}
