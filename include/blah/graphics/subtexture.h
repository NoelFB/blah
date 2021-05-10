#pragma once
#include <blah/graphics/texture.h>
#include <blah/numerics/rect.h>

namespace Blah
{
	// Subtexture is a view into a texture, and can be used to 
	// easily represent sprites in a larger texture atlas.
	struct Subtexture
	{
		// Reference to our Texture
		TextureRef texture;

		// Source rectangle, in pixels
		Rect source;

		// Frame rectangle, in pixels. This describes padding around the image.
		// This is useful for drawing images that have been trimmed. Ex. if the source
		// is 32,32, but the original image was 64,64, the frame could be -16,-16,64,64
		Rect frame;

		// `draw_coords` are automatically assigned through `update` method
		Vec2 draw_coords[4];

		// `tex_coords` are automatically assigned through the `update` method
		Vec2 tex_coords[4];

		Subtexture();
		Subtexture(const TextureRef& texture);
		Subtexture(const TextureRef& texture, Rect source);
		Subtexture(const TextureRef& texture, Rect source, Rect frame);

		// Returns the width of the image
		float width() const { return frame.w; }

		// Returns the height of the image
		float height() const { return frame.h; }

		// updates the `draw_coords` and `tex_coords`
		void update();

		// returns resulting source and frame rectangles based on the provided clip rectangle
		void crop_info(const Rect& clip, Rect* dest_source, Rect* dest_frame) const;

		// returns a subtexture cropped to the provided rectangle
		Subtexture crop(const Rect& clip) const;
	};
}