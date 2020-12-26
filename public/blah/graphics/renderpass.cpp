#include <blah/graphics/renderpass.h>
#include <blah/internal/graphics_backend.h>
#include <blah/log.h>

using namespace Blah;

RenderPass::RenderPass()
{
	blend = BlendMode::Normal;
	target = App::backbuffer;
	mesh = MeshRef();
	material = MaterialRef();
	has_viewport = false;
	has_scissor = false;
	viewport = Rect();
	scissor = Rect();
	index_start = 0;
	index_count = 0;
	instance_count = 0;
	depth = Compare::None;
	cull = Cull::None;
}

void RenderPass::perform()
{
	// Validate Material
	if (!material)
	{
		Log::warn("Trying to draw with an invalid Material; skipping render pass");
		return;
	}

	// Validate Shader
	if (!material->shader())
	{
		Log::warn("Trying to draw with an invalid Shader; skipping render pass");
		return;
	}

	// Validate Mesh
	if (!mesh)
	{
		Log::warn("Trying to draw with an invalid Mesh; skipping render pass");
		return;
	}

	// copy call
	RenderPass pass = *this;

	// Validate Backbuffer
	if (!pass.target)
	{
		pass.target = App::backbuffer;
		Log::warn("Trying to draw with an invalid Target; falling back to Back Buffer");
	}

	// Validate Index Count
	int64_t index_count = pass.mesh->index_count();
	if (pass.index_start + pass.index_count > index_count)
	{
		Log::warn(
			"Trying to draw more indices than exist in the index buffer (%i-%i / %i); trimming extra indices",
			pass.index_start,
			pass.index_start + pass.index_count,
			index_count);

		if (pass.index_start > pass.index_count)
			return;

		pass.index_count = pass.index_count - pass.index_start;
	}

	// Validate Instance Count
	int64_t instance_count = pass.mesh->instance_count();
	if (pass.instance_count > instance_count)
	{
		Log::warn(
			"Trying to draw more instances than exist in the index buffer (%i / %i); trimming extra instances",
			pass.instance_count,
			instance_count);

		pass.instance_count = instance_count;
	}

	// get the total drawable size
	Vec2 draw_size;
	if (!pass.target)
		draw_size = Vec2(App::draw_width(), App::draw_height());
	else
		draw_size = Vec2(pass.target->width(), pass.target->height());

	// Validate Viewport
	if (!pass.has_viewport)
	{
		pass.viewport.x = 0;
		pass.viewport.y = 0;
		pass.viewport.w = draw_size.x;
		pass.viewport.h = draw_size.y;
	}
	else
	{
		pass.viewport = pass.viewport.overlap_rect(Rect(0, 0, draw_size.x, draw_size.y));
	}

	// Validate Scissor
	if (pass.has_scissor)
		pass.scissor = pass.scissor.overlap_rect(Rect(0, 0, draw_size.x, draw_size.y));

	// perform render
	GraphicsBackend::render(pass);
}