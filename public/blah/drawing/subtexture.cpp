#include <blah/drawing/subtexture.h>

using namespace Blah;

Subtexture::Subtexture() {}

Subtexture::Subtexture(const TextureRef& texture)
	: Subtexture(texture, Rect(0, 0, texture->width(), texture->height())) {}

Subtexture::Subtexture(const TextureRef& texture, Rect source)
	: Subtexture(texture, source, Rect(0, 0, source.w, source.h)) {}

Subtexture::Subtexture(const TextureRef& texture, Rect source, Rect frame)
	: texture(texture), source(source), frame(frame)
{
	update();
}

void Subtexture::update()
{
	draw_coords[0].x = -frame.x;
	draw_coords[0].y = -frame.y;
	draw_coords[1].x = -frame.x + source.w;
	draw_coords[1].y = -frame.y;
	draw_coords[2].x = -frame.x + source.w;
	draw_coords[2].y = -frame.y + source.h;
	draw_coords[3].x = -frame.x;
	draw_coords[3].y = -frame.y + source.h;

	if (texture)
	{
		float uvx = 1.0f / (float)texture->width();
		float uvy = 1.0f / (float)texture->height();

		tex_coords[0].x = source.x * uvx;
		tex_coords[0].y = source.y * uvy;
		tex_coords[1].x = (source.x + source.w ) * uvx;
		tex_coords[1].y = source.y * uvy;
		tex_coords[2].x = (source.x + source.w) * uvx;
		tex_coords[2].y = (source.y + source.h) * uvy;
		tex_coords[3].x = source.x * uvx;
		tex_coords[3].y = (source.y + source.h) * uvy;
	}
}
