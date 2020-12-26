#pragma once
#include <blah/graphics/texture.h>
#include <blah/containers/stackvector.h>
#include <blah/math/color.h>
#include <memory>

// 4 color attachments + 1 depth/stencil
#define BLAH_ATTACHMENTS 5

namespace Blah
{
	typedef StackVector<TextureRef, BLAH_ATTACHMENTS> Attachments;

	class FrameBuffer;
	typedef std::shared_ptr<FrameBuffer> FrameBufferRef;

	class FrameBuffer
	{
	public:

		// Creates a new FrameBuffer with a single Color attachment
		// If the FrameBuffer creation fails, it will return an invalid FrameBufferRef.
		static FrameBufferRef create(int width, int height);

		// Creates a new FrameBuffer with the given Texture Attachments. You must provide at least one Attachment.
		// If the FrameBuffer creation fails, it will return an invalid FrameBufferRef.
		static FrameBufferRef create(int width, int height, const TextureFormat* attachments, int attachmentCount);

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

		// Clears the FrameBuffer
		virtual void clear(Color color) = 0;
	};

}
