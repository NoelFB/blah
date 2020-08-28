#include <blah/graphics/graphics.h>
#include <blah/graphics/texture.h>
#include <blah/graphics/framebuffer.h>
#include <blah/graphics/mesh.h>
#include <blah/graphics/material.h>
#include <blah/graphics/shader.h>
#include <blah/log.h>
#include <blah/internal/graphics.h>
#include <blah/images/image.h>
#include <string.h>

using namespace Blah;

namespace
{
	// active graphics device
	Internal::GraphicsDevice* device;

	// active graphics device info
	Internal::GraphicsDeviceInfo* device_info;

	// list of possible device info
	Internal::GraphicsDeviceInfo* device_options[] =
	{
		&Internal::OpenGL_DeviceInfo
	};
}

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

GfxAPI Internal::Graphics::pick_api()
{
	for (int i = 0; i < (int)GfxAPI::Count; i++)
	{
		if (device_options[i]->supported())
			return device_options[i]->api;
	}

	return GfxAPI::Any;
}

bool Internal::Graphics::init(GfxAPI api)
{
	for (int i = 0; i < (int)GfxAPI::Count; i++)
	{
		if (device_options[i]->api == api)
		{
			device_info = device_options[i];
			device = device_info->create();
			if (device != nullptr)
			{
				device->startup();
				if (device->valid)
					break;

				device_info->destroy(device);
				device = nullptr;
			}
		}
	}

	return device != nullptr && device->valid;
}

const GraphicsInfo* Graphics::info()
{
	if (device == nullptr || !device->valid)
		return nullptr;
	return &device->info;
}

void Internal::Graphics::shutdown()
{
	if (device != nullptr && device_info != nullptr)
	{
		device->shutdown();
		device_info->destroy(device);
		device = nullptr;
		device_info = nullptr;
	}
}

void Internal::Graphics::frame()
{
	if (device != nullptr && device->valid)
		device->update();
}

void Internal::Graphics::before_render()
{
	if (device != nullptr && device->valid)
		device->before_render();
}

void Internal::Graphics::after_render()
{
	if (device != nullptr && device->valid)
		device->after_render();
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
	BLAH_ASSERT(device != nullptr && device->valid, "The graphics device has not been created");

	return device->create_texture(width, height, TextureFilter::Linear, TextureWrap::Repeat, TextureWrap::Repeat, format);
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
	BLAH_ASSERT(device != nullptr && device->valid, "The graphics device has not been created");

	return device->create_framebuffer(width, height, attachments, attachment_count);
}

ShaderRef Graphics::create_shader(const ShaderData* data)
{
	BLAH_ASSERT(device != nullptr && device->valid, "The graphics device has not been created");
	return device->create_shader(data);
}

MaterialRef Graphics::create_material(const ShaderRef& shader)
{
	BLAH_ASSERT(device != nullptr && device->valid, "The graphics device has not been created");
	BLAH_ASSERT(shader && shader->is_valid(), "The provided shader is invalid");

	// TODO:
	// use a pool for Materials
	return MaterialRef(new Material(shader));
}

MeshRef Graphics::create_mesh()
{
	BLAH_ASSERT(device != nullptr && device->valid, "Graphics device has not been created");
	return device->create_mesh();
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
	BLAH_ASSERT(device != nullptr && device->valid, "Graphics device has not been created");

	// Validate Material
	if (!render_call.material || !render_call.material->is_valid())
	{
		Log::warn("Trying to draw with an invalid Material");
		return;
	}

	// Validate Shader
	if (!render_call.material->shader() || !render_call.material->shader()->is_valid())
	{
		Log::warn("Trying to draw with an invalid Shader");
		return;
	}

	// Validate Mesh
	if (!render_call.mesh || !render_call.mesh->is_valid())
	{
		Log::warn("Trying to draw with an invalid Mesh");
		return;
	}

	// Validate FrameBuffer
	if (render_call.target && !render_call.target->is_valid())
	{
		Log::warn("Trying to draw with an invalid FrameBuffer");
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
	
	device->render(&call);
}

void Graphics::clear(const FrameBufferRef& target, uint32_t rgba)
{
	BLAH_ASSERT(device != nullptr && device->valid, "Graphics device has not been created");
	device->clear(target, rgba);
}