#include <blah/graphics/texture.h>
#include <blah/images/image.h>
#include <blah/streams/stream.h>
#include <blah/internal/graphics_backend.h>
#include <blah/log.h>

using namespace Blah;

TextureRef Texture::create(const Image& image)
{
	auto tex = create(image.width, image.height, TextureFormat::RGBA);
	if (tex)
		tex->set_data((unsigned char*)image.pixels);
	return tex;
}

TextureRef Texture::create(int width, int height, unsigned char* rgba)
{
	auto tex = create(width, height, TextureFormat::RGBA);
	if (tex)
		tex->set_data(rgba);
	return tex;
}

TextureRef Texture::create(int width, int height, TextureFormat format)
{
	BLAH_ASSERT(width > 0 && height > 0, "Texture width and height must be larger than 0");
	BLAH_ASSERT((int)format > (int)TextureFormat::None && (int)format < (int)TextureFormat::Count, "Invalid texture format");

	if (width > 0 && height > 0)
		return GraphicsBackend::create_texture(width, height, format);

	return TextureRef();
}

TextureRef Texture::create(Stream& stream)
{
	Image img = Image(stream);

	if (img.pixels && img.width > 0 && img.height > 0)
	{
		auto tex = create(img.width, img.height, TextureFormat::RGBA);
		if (tex)
			tex->set_data((unsigned char*)img.pixels);
		return tex;
	}

	return TextureRef();
}

TextureRef Texture::create(const char* file)
{
	Image img = Image(file);

	if (img.pixels)
	{
		auto tex = create(img.width, img.height, TextureFormat::RGBA);
		if (tex)
			tex->set_data((unsigned char*)img.pixels);
		return tex;
	}

	return TextureRef();
}