#pragma once
#include <blah/common.h>
#include <blah/filesystem.h>
#include <blah/math/color.h>

namespace Blah
{
	enum class TextureFormat
	{
		// Invalid Format
		None,

		// Single 8-bit channe;
		R,

		// 2 8-bit channels
		RG,

		// 4 8-bit channels
		RGBA,

		// Depth 24, Stencil 8
		DepthStencil,

		// Total Formats
		Count
	};

	class Image;
	class Stream;
	class Texture;
	using TextureRef = Ref<Texture>;

	// A 2D Texture held by the GPU to be used during rendering
	class Texture
	{
	protected:
		Texture() = default;

	public:
		// Copy / Moves not allowed
		Texture(const Texture&) = delete;
		Texture(Texture&&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture& operator=(Texture&&) = delete;

		// Default Destructor
		virtual ~Texture() = default;

		// Creates a new Texture.
		// If the Texture creation fails, it will return an invalid TextureRef.
		static TextureRef create(const Image& image);

		// Creates a new Texture.
		// If image data is provided, it should be the full size of the texture.
		// If the Texture creation fails, it will return an invalid TextureRef.
		static TextureRef create(int width, int height, TextureFormat format, unsigned char* data = nullptr);

		// Creates a new Texture from a Stream.
		// If the Texture creation fails, it will return an invalid TextureRef.
		static TextureRef create(Stream& stream);

		// Creates a new Texture from a File.
		// If the Texture creation fails, it will return an invalid TextureRef.
		static TextureRef create(const FilePath& file);

		// gets the width of the texture
		virtual int width() const = 0;

		// gets the height of the texture
		virtual int height() const = 0;

		// Gets the format of the Texture
		virtual TextureFormat format() const = 0;

		// Sets the data of the Texture.
		// Note that the data should be the same format and size as the Texture. There is no row padding.
		virtual void set_data(const u8* data) = 0;

		// Sets the data of the Texture to the provided Color buffer.
		// If the Texture Format is not RGBA, this won't do anything.
		void set_data(const Color* data);

		// Gets the data of the Texture.
		// Note that the data will be written to in the same format as the Texture,
		// and you should allocate enough space for the full texture. There is no row padding.
		virtual void get_data(u8* data) = 0;

		// Gets the data of the Texture.
		// If the Texture Format is not RGBA, this won't do anything.
		void get_data(Color* data);

		// Returns true if the Texture is part of a FrameBuffer
		virtual bool is_framebuffer() const = 0;
	};
}