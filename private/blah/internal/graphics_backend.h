#pragma once
#include <blah/graphics/graphics.h>

namespace Blah
{
	// Graphics backend API used for rendering.
	// All rendering ends up going through here.
	namespace GraphicsBackend
	{
		// Initializes the graphics backend
		bool init();

		// Shuts down the graphics backend
		void shutdown();

		// Returns info about the renderer
		const GraphicsInfo* info();

		// Returns the renderer type
		GraphicsRenderer renderer();

		// Called once per frame
		void frame();

		// Called before rendering begins
		void before_render();

		// Called after renderings ends
		void after_render();

		// Performs a draw call
		void render(const RenderCall& call);

		// Clears a buffer
		void clear(const FrameBufferRef& target, uint32_t rgba);

		// Creates a new Texture.
		// if the Texture is invalid, this should return an empty reference.
		TextureRef create_texture(int width, int height, TextureFilter filter, TextureWrap wrap_x, TextureWrap wrap_y, TextureFormat format);

		// Creates a new FrameBuffer.
		// if the FrameBuffer is invalid, this should return an empty reference.
		FrameBufferRef create_framebuffer(int width, int height, const TextureFormat* attachments, int attachmentCount);

		// Creates a new Shader.
		// if the Shader is invalid, this should return an empty reference.
		ShaderRef create_shader(const ShaderData* data);

		// Creates a new Mesh.
		// if the Mesh is invalid, this should return an empty reference.
		MeshRef create_mesh();
	}
}