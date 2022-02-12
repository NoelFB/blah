#pragma once
#include <blah/app.h>
#include <blah/graphics/renderpass.h>
#include <blah/graphics/texture.h>
#include <blah/graphics/target.h>
#include <blah/graphics/shader.h>
#include <blah/graphics/mesh.h>
#include <blah/graphics/material.h>
#include <blah/math/color.h>

#define BLAH_ASSERT_RENDERER() BLAH_ASSERT(Renderer::instance, "Renderer has not been created")

namespace Blah
{
	class Renderer
	{
	public:

		// Reference to the current Renderer in use
		inline static Renderer* instance = nullptr;

		// Renderer Features
		RendererFeatures features;

		// Default Shader for the Batcher
		ShaderRef default_batcher_shader;

		virtual ~Renderer() = default;

		// Initialize the Graphics
		virtual bool init() = 0;

		// Shutdown the Graphics
		virtual void shutdown() = 0;

		// Called once per frame
		virtual void update() = 0;

		// Called before rendering begins
		virtual void before_render() = 0;

		// Called after renderings ends
		virtual void after_render() = 0;

		// Performs a draw call
		virtual void render(const RenderPass& pass) = 0;

		// Clears the backbuffer
		virtual void clear_backbuffer(Color color, float depth, u8 stencil, ClearMask mask) = 0;

		// Creates a new Texture.
		// if the Texture is invalid, this should return an empty reference.
		virtual TextureRef create_texture(int width, int height, TextureFormat format) = 0;

		// Creates a new Target.
		// if the Target is invalid, this should return an empty reference.
		virtual TargetRef create_target(int width, int height, const TextureFormat* attachments, int attachment_count) = 0;

		// Creates a new Shader.
		// if the Shader is invalid, this should return an empty reference.
		virtual ShaderRef create_shader(const ShaderData* data) = 0;

		// Creates a new Mesh.
		// if the Mesh is invalid, this should return an empty reference.
		virtual MeshRef create_mesh() = 0;

	private:
		static Renderer* try_make_opengl();
		static Renderer* try_make_d3d11();

	public:
		static Renderer* try_make_renderer(RendererType type)
		{
			switch (type)
			{
			case RendererType::None: return nullptr;
			case RendererType::OpenGL: return try_make_opengl();
			case RendererType::D3D11: return try_make_d3d11();
			}

			return nullptr;
		}

		static RendererType default_type()
		{
#if defined(BLAH_RENDERER_D3D11) && defined(_WIN32)
			return RendererType::D3D11;
#else
			return RendererType::OpenGL;
#endif
		}
	};
}