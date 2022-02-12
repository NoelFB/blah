#include <blah/graphics/texture.h>
#include <blah/images/image.h>
#include <blah/streams/stream.h>
#include <blah/common.h>
#include "../internal/internal.h"

using namespace Blah;

TextureRef Texture::create(const Image& image)
{
	return create(image.width, image.height, TextureFormat::RGBA, (unsigned char*)image.pixels);
}

TextureRef Texture::create(int width, int height, TextureFormat format, unsigned char* data)
{
	BLAH_ASSERT_RENDERER();
	BLAH_ASSERT(width > 0 && height > 0, "Texture width and height must be larger than 0");
	BLAH_ASSERT((int)format > (int)TextureFormat::None && (int)format < (int)TextureFormat::Count, "Invalid texture format");

	if (App::Internal::renderer)
	{
		auto tex = App::Internal::renderer->create_texture(width, height, format);

		if (tex && data != nullptr)
			tex->set_data(data);

		return tex;
	}

	return TextureRef();
}

TextureRef Texture::create(Stream& stream)
{
	return create(Image(stream));
}

TextureRef Texture::create(const FilePath& file)
{
	return create(Image(file));
}

void Texture::set_data(const Color* data)
{
	if (format() == TextureFormat::RGBA)
		set_data((u8*)data);
}

void Texture::get_data(Color* data)
{
	if (format() == TextureFormat::RGBA)
		get_data((u8*)data);
}
