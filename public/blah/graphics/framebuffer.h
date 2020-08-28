#pragma once
#include <blah/graphics/graphics.h>
#include <blah/graphics/texture.h>
#include <blah/containers/stacklist.h>
#include <memory>

namespace Blah
{
	typedef StackList<TextureRef, BLAH_ATTACHMENTS> Attachments;

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		// Gets the list of Attachments from the FrameBuffer
		virtual Attachments& attachments() = 0;

		// Gets the list of Attachments from the FrameBuffer
		virtual const Attachments& attachments() const = 0;

		// Gets the width of the FrameBuffer
		virtual int width() const = 0;

		// Gets the height of the FrameBuffer
		virtual int height() const = 0;

		// Returns true if the FrameBuffer is valid
		virtual bool is_valid() const = 0;

		// Destroys the given FrameBuffer
		virtual void dispose() = 0;
	};

	typedef std::shared_ptr<FrameBuffer> FrameBufferRef;
}
