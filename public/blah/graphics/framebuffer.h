#pragma once
#include <blah/graphics/texture.h>
#include <blah/containers/stackvector.h>
#include <memory>

// 4 color attachments + 1 depth/stencil
#define BLAH_ATTACHMENTS 5

namespace Blah
{
	typedef StackVector<TextureRef, BLAH_ATTACHMENTS> Attachments;

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		// Gets the list of Attachments from the FrameBuffer
		virtual Attachments& attachments() = 0;

		// Gets the list of Attachments from the FrameBuffer
		virtual const Attachments& attachments() const = 0;

		// Gets the Attachment at a given index from the FrameBuffer
		virtual TextureRef& attachment(int index) = 0;

		// Gets the Attachment at a given index from the FrameBuffer
		virtual const TextureRef& attachment(int index) const = 0;

		// Gets the width of the FrameBuffer
		virtual int width() const = 0;

		// Gets the height of the FrameBuffer
		virtual int height() const = 0;
	};

	typedef std::shared_ptr<FrameBuffer> FrameBufferRef;
}
