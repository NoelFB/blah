#pragma once
#include <blah_graphics.h>
#include <blah_spatial.h>

namespace Blah
{
	// Subtexture is a view into a texture, and can be used to 
	// easily represent sprites in a larger texture atlas.
	struct Subtexture
	{
		// Reference to our Texture
		TextureRef texture;

		// Source rectangle, in pixels
		Rectf source;

		// Frame rectangle, in pixels. This describes padding around the image.
		// This is useful for drawing images that have been trimmed. Ex. if the source
		// is 32,32, but the original image was 64,64, the frame could be -16,-16,64,64
		Rectf frame;

		// `draw_coords` are automatically assigned through `update` method
		Vec2f draw_coords[4];

		// `tex_coords` are automatically assigned through the `update` method
		Vec2f tex_coords[4];

		Subtexture();
		Subtexture(const TextureRef& texture);
		Subtexture(const TextureRef& texture, Rectf source);
		Subtexture(const TextureRef& texture, Rectf source, Rectf frame);

		// Returns the width of the image
		float width() const { return frame.w; }

		// Returns the height of the image
		float height() const { return frame.h; }

		// updates the `draw_coords` and `tex_coords`
		void update();

		// returns resulting source and frame rectangles based on the provided clip rectangle
		void crop_info(const Rectf& clip, Rectf* dest_source, Rectf* dest_frame) const;

		// returns a subtexture cropped to the provided rectangle
		Subtexture crop(const Rectf& clip) const;
	};
}