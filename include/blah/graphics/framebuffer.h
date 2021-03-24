#pragma once
#include <blah/graphics/texture.h>
#include <blah/containers/stackvector.h>
#include <blah/math/color.h>
#include <memory>

namespace Blah
{
	enum class ClearMask
	{
		None = 0,
		Color = 1,
		Depth = 2,
		Stencil = 4,
		All = (int)Color | (int)Depth | (int)Stencil
	};

	// Up to 4 color attachments + 1 depth/stencil
	using Attachments = StackVector<TextureRef, 5>;
	using AttachmentFormats = StackVector<TextureFormat, 5>;

	class FrameBuffer;
	using FrameBufferRef = std::shared_ptr<FrameBuffer>;

	// FrameBuffer is a 2D Buffer that can be drawn to.
	// It can hold up to 4 color Textures, and 1 Depth/Stencil Texture.
	class FrameBuffer
	{
	protected:
		FrameBuffer() = default;

	public:
		// Copy / Moves not allowed
		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer(FrameBuffer&&) = delete;
		FrameBuffer& operator=(const FrameBuffer&) = delete;
		FrameBuffer& operator=(FrameBuffer&&) = delete;

		// Default Destructor
		virtual ~FrameBuffer() = default;

		// Creates a new FrameBuffer with a single Color attachment
		// If the FrameBuffer creation fails, it will return an invalid FrameBufferRef.
		static FrameBufferRef create(int width, int height);

		// Creates a new FrameBuffer with the given Texture Attachments. You must provide at least one Attachment.
		// If the FrameBuffer creation fails, it will return an invalid FrameBufferRef.
		static FrameBufferRef create(int width, int height, const AttachmentFormats& attachments);

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
		virtual void clear(Color color = Color::black, float depth = 1.0f, u8 stencil = 0, ClearMask mask = ClearMask::All) = 0;
	};

}
