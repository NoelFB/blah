#include <blah/graphics/framebuffer.h>
#include "../internal/graphics_backend.h"

using namespace Blah;

FrameBufferRef FrameBuffer::create(int width, int height)
{
	return create(width, height, { TextureFormat::RGBA });
}

FrameBufferRef FrameBuffer::create(int width, int height, const AttachmentFormats& attachments)
{
	BLAH_ASSERT(width > 0 && height > 0, "FrameBuffer width and height must be larger than 0");
	BLAH_ASSERT(attachments.size() > 0, "At least one attachment must be provided");

	int color_count = 0;
	int depth_count = 0;

	for (int i = 0; i < attachments.size(); i++)
	{
		BLAH_ASSERT((int)attachments[i] > (int)TextureFormat::None && (int)attachments[i] < (int)TextureFormat::Count, "Invalid texture format");

		if (attachments[i] == TextureFormat::DepthStencil)
			depth_count++;
		else
			color_count++;
	}

	BLAH_ASSERT(depth_count <= 1, "FrameBuffer can only have 1 Depth/Stencil Texture");
	BLAH_ASSERT(color_count <= Attachments::MaxCapacity - 1, "Exceeded maximum Color attachment count");

	return GraphicsBackend::create_framebuffer(width, height, attachments.data(), attachments.size());
}
