#include <blah/graphics/target.h>
#include "../internal/internal.h"

using namespace Blah;

TargetRef Target::create(int width, int height)
{
	AttachmentFormats formats;
	formats.push_back(TextureFormat::RGBA);
	return create(width, height, formats);
}

TargetRef Target::create(int width, int height, const AttachmentFormats& textures)
{
	BLAH_ASSERT_RENDERER();
	BLAH_ASSERT(width > 0 && height > 0, "Target width and height must be larger than 0");
	BLAH_ASSERT(textures.size() > 0, "At least one texture must be provided");

	int color_count = 0;
	int depth_count = 0;

	for (int i = 0; i < textures.size(); i++)
	{
		BLAH_ASSERT((int)textures[i] > (int)TextureFormat::None && (int)textures[i] < (int)TextureFormat::Count, "Invalid texture format");

		if (textures[i] == TextureFormat::DepthStencil)
			depth_count++;
		else
			color_count++;
	}

	BLAH_ASSERT(depth_count <= 1, "Target can only have 1 Depth/Stencil Texture");
	BLAH_ASSERT(color_count <= Attachments::capacity - 1, "Exceeded maximum Color texture count");

	if (App::Internal::renderer)
		return App::Internal::renderer->create_target(width, height, textures.data(), textures.size());

	return TargetRef();
}

TextureRef& Target::texture(int index)
{
	return textures()[index];
}

const TextureRef& Target::texture(int index) const
{
	return textures()[index];
}

int Target::width() const
{
	return textures()[0]->width();
}

int Target::height() const
{
	return textures()[0]->height();
}
