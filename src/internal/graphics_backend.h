#pragma once
#include <blah/core/app.h>
#include <blah/graphics/renderpass.h>
#include <blah/graphics/texture.h>
#include <blah/graphics/framebuffer.h>
#include <blah/graphics/shader.h>
#include <blah/graphics/mesh.h>
#include <blah/graphics/material.h>
#include <blah/math/color.h>

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
		const RendererFeatures& features();

		// Returns the renderer type
		Renderer renderer();

		// Called once per frame
		void frame();

		// Called before rendering begins
		void before_render();

		// Called after renderings ends
		void after_render();

		// Performs a draw call
		void render(const RenderPass& pass);

		// Clears the backbuffer
		void clear_backbuffer(Color color);

		// Creates a new Texture.
		// if the Texture is invalid, this should return an empty reference.
		TextureRef create_texture(int width, int height, TextureFormat format);

		// Creates a new FrameBuffer.
		// if the FrameBuffer is invalid, this should return an empty reference.
		FrameBufferRef create_framebuffer(int width, int height, const TextureFormat* attachments, int attachment_count);

		// Creates a new Shader.
		// if the Shader is invalid, this should return an empty reference.
		ShaderRef create_shader(const ShaderData* data);

		// Creates a new Mesh.
		// if the Mesh is invalid, this should return an empty reference.
		MeshRef create_mesh();
	}
}