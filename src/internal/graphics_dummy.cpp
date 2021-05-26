#if !(defined(BLAH_GRAPHICS_OPENGL) || defined(BLAH_GRAPHICS_D3D11))

#include "graphics.h"
#include "platform.h"
#include <blah/common.h>

namespace Blah
{
	class Dummy_Texture : public Texture
	{
	private:
		int m_width;
		int m_height;
		TextureFormat m_format;
		bool m_framebuffer;

	public:

		Dummy_Texture(int width, int height, TextureFormat format, bool framebuffer)
		{
			m_width = width;
			m_height = height;
			m_format = format;
			m_framebuffer = framebuffer;
		}

		virtual int width() const override
		{
			return m_width;
		}

		virtual int height() const override
		{
			return m_height;
		}

		virtual TextureFormat format() const override
		{
			return m_format;
		}

		virtual void set_data(unsigned char* data) override
		{

		}

		virtual void get_data(unsigned char* data) override
		{

		}

		virtual bool is_framebuffer() const override
		{
			return m_framebuffer;
		}

	};

	class Dummy_Target : public Target
	{
	private:
		Attachments m_attachments;

	public:

		Dummy_Target(int width, int height, const TextureFormat* attachments, int attachmentCount)
		{
			for (int i = 0; i < attachmentCount; i++)
			{
				m_attachments.push_back(
					TextureRef(new Dummy_Texture(width, height, attachments[i], true))
				);
			}
		}

		virtual Attachments& textures() override
		{
			return m_attachments;
		}

		virtual const Attachments& textures() const override
		{
			return m_attachments;
		}

		virtual void clear(Color color) override
		{

		}
	};

	class Dummy_Shader : public Shader
	{
	private:
		Vector<UniformInfo> m_uniforms;

	public:
		Dummy_Shader(const ShaderData* data)
		{

		}

		virtual Vector<UniformInfo>& uniforms() override
		{
			return m_uniforms;
		}

		virtual const Vector<UniformInfo>& uniforms() const override
		{
			return m_uniforms;
		}
	};

	class Dummy_Mesh : public Mesh
	{
	private:
		i64 m_index_count = 0;
		i64 m_vertex_count = 0;
		i64 m_instance_count = 0;
	public:

		Dummy_Mesh()
		{

		}

		virtual void index_data(IndexFormat format, const void* indices, i64 count) override
		{
			m_index_count = count;
		}

		virtual void vertex_data(const VertexFormat& format, const void* vertices, i64 count) override
		{
			m_vertex_count = count;
		}

		virtual void instance_data(const VertexFormat& format, const void* instances, i64 count) override
		{
			m_instance_count = count;
		}

		virtual i64 index_count() const override
		{
			return m_index_count;
		}

		virtual i64 vertex_count() const override
		{
			return m_vertex_count;
		}

		virtual i64 instance_count() const override
		{
			return m_instance_count;
		}
	};

	bool Graphics::init()
	{
		Log::info("Dummy Renderer");
		return true;
	}

	Renderer Graphics::renderer()
	{
		return Renderer::None;
	}

	void Graphics::shutdown()
	{

	}

	const RendererFeatures& Graphics::features()
	{
		static const RendererFeatures features{ false, true, 4096 };
		return features;
	}

	void Graphics::update() {}
	void Graphics::before_render() {}
	void Graphics::after_render() {}

	TextureRef Graphics::create_texture(int width, int height, TextureFormat format)
	{
		return TextureRef(new Dummy_Texture(width, height, format, false));
	}

	TargetRef Graphics::create_target(int width, int height, const TextureFormat* attachments, int attachmentCount)
	{
		return TargetRef(new Dummy_Target(width, height, attachments, attachmentCount));
	}

	ShaderRef Graphics::create_shader(const ShaderData* data)
	{
		return ShaderRef(new Dummy_Shader(data));
	}

	MeshRef Graphics::create_mesh()
	{
		return MeshRef(new Dummy_Mesh());
	}

	void Graphics::render(const RenderPass& pass)
	{

	}

	void Graphics::clear_backbuffer(Color color)
	{

	}
}

#endif // !(defined(BLAH_GRAPHICS_OPENGL) || defined(BLAH_GRAPHICS_D3D11))
