#pragma once
#include <blah/common.h>
#include <blah/graphics/texture.h>
#include <blah/containers/stackvector.h>
#include <blah/numerics/color.h>

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

	// Up to 4 color textures + 1 depth/stencil
	using Attachments = StackVector<TextureRef, 5>;
	using AttachmentFormats = StackVector<TextureFormat, 5>;

	class Target;
	using TargetRef = Ref<Target>;

	// Target is a 2D Buffer that can be drawn to.
	// It can hold up to 4 color Textures, and 1 Depth/Stencil Texture.
	class Target
	{
	protected:
		Target() = default;

	public:
		// Copy / Moves not allowed
		Target(const Target&) = delete;
		Target(Target&&) = delete;
		Target& operator=(const Target&) = delete;
		Target& operator=(Target&&) = delete;

		// Default Destructor
		virtual ~Target() = default;

		// Creates a new Target with a single Color texture
		// If the Target creation fails, it will return an invalid TargetRef.
		static TargetRef create(int width, int height);

		// Creates a new Target with the given Texture Attachments. You must provide at least one Attachment.
		// If the Target creation fails, it will return an invalid TargetRef.
		static TargetRef create(int width, int height, const AttachmentFormats& textures);

		// Gets the list of Attachments from the Target
		virtual Attachments& textures() = 0;

		// Gets the list of Attachments from the Target
		virtual const Attachments& textures() const = 0;

		// Gets the Attachment at a given index from the Target
		TextureRef& texture(int index);

		// Gets the Attachment at a given index from the Target
		const TextureRef& texture(int index) const;

		// Gets the width of the Target
		virtual int width() const;

		// Gets the height of the Target
		virtual int height() const;

		// Clears the Target
		virtual void clear(Color color = Color::black, float depth = 1.0f, u8 stencil = 0, ClearMask mask = ClearMask::All) = 0;
	};

}
