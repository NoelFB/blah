#pragma once
#include <blah/graphics/texture.h>
#include <blah/math/rect.h>

namespace Blah
{
	struct Subtexture
	{
		TextureRef	texture;
		Rect		source;
		Rect		frame;
		Vec2		draw_coords[4];	// draw coords are automatically assigned through Update method
		Vec2		tex_coords[4];	// tex coords are automatically assigned through Update method

		Subtexture();
		Subtexture(const TextureRef& texture, Rect source, Rect frame);

		float width() const { return frame.w; }
		float height() const { return frame.h; }

		// updates the draw and tex coords
		void update();
	};
}