#pragma once
#include <blah/app.h>
#include <blah/graphics/renderpass.h>
#include <blah/graphics/texture.h>
#include <blah/graphics/target.h>
#include <blah/graphics/shader.h>
#include <blah/graphics/mesh.h>
#include <blah/graphics/material.h>
#include <blah/numerics/color.h>

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
		void clear_backbuffer(Color color, float depth, u8 stencil, ClearMask mask);

		// Creates a new Texture.
		// if the Texture is invalid, this should return an empty reference.
		TextureRef create_texture(int width, int height, TextureFormat format);

		// Creates a new Target.
		// if the Target is invalid, this should return an empty reference.
		TargetRef create_target(int width, int height, const TextureFormat* attachments, int attachment_count);

		// Creates a new Shader.
		// if the Shader is invalid, this should return an empty reference.
		ShaderRef create_shader(const ShaderData* data);

		// Creates a new Mesh.
		// if the Mesh is invalid, this should return an empty reference.
		MeshRef create_mesh();
	}
}