#include <blah/graphics/graphics.h>
#include <blah/graphics/texture.h>
#include <blah/graphics/framebuffer.h>
#include <blah/graphics/mesh.h>
#include <blah/graphics/material.h>
#include <blah/graphics/shader.h>
#include <blah/log.h>
#include <blah/internal/graphics_backend.h>
#include <blah/images/image.h>
#include <string.h>

using namespace Blah;

const BlendMode BlendMode::Normal = BlendMode(
	BlendOp::Add,
	BlendFactor::One,
	BlendFactor::OneMinusSrcAlpha,
	BlendOp::Add,
	BlendFactor::One,
	BlendFactor::OneMinusSrcAlpha,
	BlendMask::RGBA,
	0xffffffff
);

const BlendMode BlendMode::Subtract = BlendMode(
	BlendOp::ReverseSubtract,
	BlendFactor::One,
	BlendFactor::One,
	BlendOp::Add,
	BlendFactor::One,
	BlendFactor::One,
	BlendMask::RGBA,
	0xffffffff
);

const GraphicsInfo* Graphics::info()
{
	return GraphicsBackend::info();
}

GraphicsRenderer Graphics::renderer()
{
	return GraphicsBackend::renderer();
}

TextureRef Graphics::create_texture(const Image& image)
{
	auto tex = create_texture(image.width, image.height, TextureFormat::RGBA);
	tex->set_data((unsigned char*)image.pixels);
	return tex;
}

TextureRef Graphics::create_texture(int width, int height, unsigned char* rgba)
{
	auto tex = create_texture(width, height, TextureFormat::RGBA);
	tex->set_data(rgba);
	return tex;
}

TextureRef Graphics::create_texture(int width, int height, TextureFormat format)
{
	BLAH_ASSERT(width > 0 && height > 0, "Texture width and height must be larger than 0");
	BLAH_ASSERT((int)format > (int)TextureFormat::None && (int)format < (int)TextureFormat::Count, "Invalid texture format");

	return GraphicsBackend::create_texture(width, height, TextureFilter::Linear, TextureWrap::Repeat, TextureWrap::Repeat, format);
}

TextureRef Graphics::create_texture(Stream& stream)
{
	Image img = Image(stream);

	if (img.pixels != nullptr && img.width > 0 && img.height > 0)
	{
		auto tex = create_texture(img.width, img.height, TextureFormat::RGBA);
		tex->set_data((unsigned char*)img.pixels);
		return tex;
	}

	return TextureRef();
}

TextureRef Graphics::create_texture(const char* file)
{
	Image img = Image(file);

	if (img.pixels != nullptr)
	{
		auto tex = create_texture(img.width, img.height, TextureFormat::RGBA);
		tex->set_data((unsigned char*)img.pixels);
		return tex;
	}
	
	return TextureRef();
}

FrameBufferRef Graphics::create_framebuffer(int width, int height)
{
	static const TextureFormat attachment = TextureFormat::RGBA;
	return create_framebuffer(width, height, &attachment, 1);
}

FrameBufferRef Graphics::create_framebuffer(int width, int height, const TextureFormat* attachments, int attachment_count)
{
	BLAH_ASSERT(width > 0 && height > 0, "FrameBuffer width and height must be larger than 0");
	BLAH_ASSERT(attachment_count <= BLAH_ATTACHMENTS, "Exceeded maximum attachment count");
	BLAH_ASSERT(attachment_count > 0, "At least one attachment must be provided");
	for (int i = 0; i < attachment_count; i++)
		BLAH_ASSERT((int)attachments[i] > (int)TextureFormat::None && (int)attachments[i] < (int)TextureFormat::Count, "Invalid texture format");

	return GraphicsBackend::create_framebuffer(width, height, attachments, attachment_count);
}

ShaderRef Graphics::create_shader(const ShaderData* data)
{
	return GraphicsBackend::create_shader(data);
}

MaterialRef Graphics::create_material(const ShaderRef& shader)
{
	BLAH_ASSERT(shader, "The provided shader is invalid");

	// TODO:
	// use a pool for Materials?
	return MaterialRef(new Material(shader));
}

MeshRef Graphics::create_mesh()
{
	return GraphicsBackend::create_mesh();
}

RenderCall::RenderCall()
{
	blend = BlendMode::Normal;
	target = Graphics::backbuffer;
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

void Graphics::render(const RenderCall& render_call)
{
	// Validate Material
	if (!render_call.material)
	{
		Log::warn("Trying to draw with an invalid Material");
		return;
	}

	// Validate Shader
	if (!render_call.material->shader())
	{
		Log::warn("Trying to draw with an invalid Shader");
		return;
	}

	// Validate Mesh
	if (!render_call.mesh)
	{
		Log::warn("Trying to draw with an invalid Mesh");
		return;
	}

	// copy call
	RenderCall call = render_call;

	// Validate Index Count
	int64_t index_count = call.mesh->index_count();
	if (call.index_start + call.index_count > index_count)
	{
		Log::warn(
			"Trying to draw more indices than exist in the index buffer (%i-%i / %i)",
			call.index_start,
			call.index_start + call.index_count,
			index_count);

		if (call.index_start > call.index_count)
			return;

		call.index_count = call.index_count - call.index_start;
	}

	// Validate Instance Count
	int64_t instance_count = call.mesh->instance_count();
	if (call.instance_count > instance_count)
	{
		Log::warn(
			"Trying to draw more instances than exist in the index buffer (%i / %i)",
			call.instance_count,
			instance_count);

		call.instance_count = instance_count;
	}

	// Validate Viewport
	if (!call.has_viewport)
	{
		Point drawSize;

		if (!call.target)
			drawSize = Point(App::draw_width(), App::draw_height());
		else
			drawSize = Point(call.target->width(), call.target->height());

		call.viewport.x = 0;
		call.viewport.y = 0;
		call.viewport.w = (float)drawSize.x;
		call.viewport.h = (float)drawSize.y;
	}
	else
	{
		if (call.viewport.x < 0)
		{
			call.viewport.w += call.viewport.x;
			call.viewport.x = 0;
		}
		if (call.viewport.y < 0)
		{
			call.viewport.h += call.viewport.y;
			call.viewport.y = 0;
		}
		if (call.viewport.w < 0)
			call.viewport.w = 0;
		if (call.viewport.h < 0)
			call.viewport.h = 0;
	}

	// Validate Scissor
	if (call.has_scissor)
	{
		if (call.scissor.x < 0)
		{
			call.scissor.w += call.scissor.x;
			call.scissor.x = 0;
		}
		if (call.scissor.y < 0)
		{
			call.scissor.h += call.scissor.y;
			call.scissor.y = 0;
		}
		if (call.scissor.w < 0)
			call.scissor.w = 0;
		if (call.scissor.h < 0)
			call.scissor.h = 0;
	}
	
	GraphicsBackend::render(&call);
}

void Graphics::clear(const FrameBufferRef& target, uint32_t rgba)
{
	GraphicsBackend::clear(target, rgba);
}