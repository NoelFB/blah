#pragma once
#include <blah/graphics/graphics.h>

namespace Blah
{
	namespace GraphicsBackend
	{
		bool init();
		void shutdown();

		const GraphicsInfo* info();
		GraphicsRenderer renderer();
		void frame();
		void before_render();
		void after_render();
		void render(RenderCall* call);
		void clear(const FrameBufferRef& target, uint32_t rgba);

		TextureRef create_texture(int width, int height, TextureFilter filter, TextureWrap wrap_x, TextureWrap wrap_y, TextureFormat format);
		FrameBufferRef create_framebuffer(int width, int height, const TextureFormat* attachments, int attachmentCount);
		ShaderRef create_shader(const ShaderData* data);
		MeshRef create_mesh();
	}
}