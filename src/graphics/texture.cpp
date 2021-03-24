#include <blah/graphics/texture.h>
#include <blah/images/image.h>
#include <blah/streams/stream.h>
#include <blah/core/common.h>
#include "../internal/graphics_backend.h"

using namespace Blah;

TextureRef Texture::create(const Image& image)
{
	return create(image.width, image.height, TextureFormat::RGBA, (unsigned char*)image.pixels);
}

TextureRef Texture::create(int width, int height, TextureFormat format, unsigned char* data)
{
	BLAH_ASSERT(width > 0 && height > 0, "Texture width and height must be larger than 0");
	BLAH_ASSERT((int)format > (int)TextureFormat::None && (int)format < (int)TextureFormat::Count, "Invalid texture format");

	auto tex = GraphicsBackend::create_texture(width, height, format);

	if (tex && data != nullptr)
		tex->set_data(data);

	return tex;
}

TextureRef Texture::create(Stream& stream)
{
	return create(Image(stream));
}

TextureRef Texture::create(const FilePath& file)
{
	return create(Image(file));
}