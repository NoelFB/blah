#pragma once
#include <memory>

namespace Blah
{
	enum class TextureFilter
	{
		None,
		Linear,
		Nearest
	};

	enum class TextureWrap
	{
		None,
		Clamp,
		Repeat
	};

	enum class TextureFormat
	{
		None,
		R,
		RG,
		RGB,
		RGBA,
		DepthStencil,
		Count
	};

	class Image;
	class Stream;
	class Texture;
	typedef std::shared_ptr<Texture> TextureRef;

	class Texture
	{
	public:

		// Creates a new Texture.
		// If the Texture creation fails, it will return an invalid TextureRef.
		static TextureRef create(const Image& image);

		// Creates a new Texture.
		// If the Texture creation fails, it will return an invalid TextureRef.
		static TextureRef create(int width, int height, unsigned char* rgba);

		// Creates a new Texture.
		// If the Texture creation fails, it will return an invalid TextureRef.
		static TextureRef create(int width, int height, TextureFormat format);

		// Creates a new Texture from a Stream.
		// If the Texture creation fails, it will return an invalid TextureRef.
		static TextureRef create(Stream& stream);

		// Creates a new Texture from a File.
		// If the Texture creation fails, it will return an invalid TextureRef.
		static TextureRef create(const char* file);

		virtual ~Texture() = default;

		// gets the width of the texture
		virtual int width() const = 0;

		// gets the height of the texture
		virtual int height() const = 0;

		// Gets the format of the Texture
		virtual TextureFormat format() const = 0;

		// Sets the filter of the Texture
		virtual void set_filter(TextureFilter filter) = 0;

		// Gets the filter of the Texture
		virtual TextureFilter get_filter() const = 0;

		// Sets the wrap of the Texture
		virtual void set_wrap(TextureWrap x, TextureWrap y) = 0;

		// Gets the wrap of the Texture
		virtual TextureWrap get_wrap_x() const = 0;

		// Gets the wrap of the Texture
		virtual TextureWrap get_wrap_y() const = 0;

		// Sets the data of the Texture.
		// Note that the pixel buffer should be in the same format as the Texture. There is no row padding.
		// If the pixel buffer isn't the same size as the texture, it will set the minimum available amount of data.
		virtual void set_data(unsigned char* data) = 0;

		// Gets the data of the Texture.
		// Note that the pixel buffer will be written to in the same format as the Texture,
		// and you should allocate enough space for the full texture. There is no row padding.
		virtual void get_data(unsigned char* data) = 0;

		// Returns true if the Texture is part of a FrameBuffer
		virtual bool is_framebuffer() const = 0;
	};
}