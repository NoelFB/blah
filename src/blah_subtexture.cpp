#include <blah_subtexture.h>
#include <blah_calc.h>

using namespace Blah;

Subtexture::Subtexture() {}

Subtexture::Subtexture(const TextureRef& texture)
	: Subtexture(texture, Rectf(0, 0, (float)texture->width(), (float)texture->height())) {}

Subtexture::Subtexture(const TextureRef& texture, Rectf source)
	: Subtexture(texture, source, Rectf(0, 0, source.w, source.h)) {}

Subtexture::Subtexture(const TextureRef& texture, Rectf source, Rectf frame)
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

void Subtexture::crop_info(const Rectf& clip, Rectf* dest_source, Rectf* dest_frame) const
{
	*dest_source = (clip + source.top_left() + frame.top_left()).overlap_rect(source);

	dest_frame->x = Calc::min(0.0f, frame.x + clip.x);
	dest_frame->y = Calc::min(0.0f, frame.y + clip.y);
	dest_frame->w = clip.w;
	dest_frame->h = clip.h;
}

Subtexture Subtexture::crop(const Rectf& clip) const
{
	Subtexture dst;
	dst.texture = texture;
	crop_info(clip, &dst.source, &dst.frame);
	dst.update();
	return dst;
}
