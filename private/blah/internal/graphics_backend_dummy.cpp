#if !(defined(BLAH_USE_OPENGL) || defined(BLAH_USE_D3D11))

#include <blah/internal/graphics_backend.h>

namespace Blah
{
	class Dummy_Texture : public Texture
	{
	private:
		int m_width;
		int m_height;
		TextureFilter m_filter;
		TextureWrap m_wrap_x;
		TextureWrap m_wrap_y;
		TextureFormat m_format;
		bool m_framebuffer;

	public:

		Dummy_Texture(int width, int height, TextureFilter filter, TextureWrap wrap_x, TextureWrap wrap_y, TextureFormat format, bool framebuffer)
		{
			m_width = width;
			m_height = height;
			m_filter = filter;
			m_wrap_x = wrap_x;
			m_wrap_y = wrap_y;
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

		virtual void set_filter(TextureFilter filter) override
		{
			m_filter = filter;
		}

		virtual TextureFilter get_filter() const override
		{
			return m_filter;
		}

		virtual void set_wrap(TextureWrap x, TextureWrap y) override
		{
			m_wrap_x = x;
			m_wrap_y = y;
		}

		virtual TextureWrap get_wrap_x() const override
		{
			return m_wrap_x;
		}

		virtual TextureWrap get_wrap_y() const override
		{
			return m_wrap_y;
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

	class Dummy_FrameBuffer : public FrameBuffer
	{
	private:
		Attachments m_attachments;

	public:

		Dummy_FrameBuffer(int width, int height, const TextureFormat* attachments, int attachmentCount)
		{
			for (int i = 0; i < attachmentCount; i++)
			{
				m_attachments.push_back(
					TextureRef(
						new Dummy_Texture(width, height,
							TextureFilter::Linear,
							TextureWrap::Repeat,
							TextureWrap::Repeat,
							attachments[i],
							true)));
			}
		}

		virtual Attachments& attachments() override
		{
			return m_attachments;
		}

		virtual const Attachments& attachments() const override
		{
			return m_attachments;
		}

		virtual TextureRef& attachment(int index) override
		{
			return m_attachments[index];
		}

		virtual const TextureRef& attachment(int index) const override
		{
			return m_attachments[index];
		}

		virtual int width() const override
		{
			return m_attachments[0]->width();
		}

		virtual int height() const override
		{
			return m_attachments[0]->height();
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
	public:

		Dummy_Mesh()
		{

		}

		virtual void index_data(IndexFormat format, const void* indices, int64_t count) override
		{

		}

		virtual void vertex_data(const VertexFormat& format, const void* vertices, int64_t count) override
		{

		}

		virtual void instance_data(const VertexFormat& format, const void* instances, int64_t count) override
		{

		}

		virtual int64_t index_count() const override
		{
			return 0;
		}

		virtual int64_t vertex_count() const override
		{
			return 0;
		}

		virtual int64_t instance_count() const override
		{
			return 0;
		}
	};

	bool GraphicsBackend::init()
	{
		Log::print("Dummy Renderer");
		return true;
	}

	Renderer GraphicsBackend::renderer()
	{
		return Renderer::None;
	}

	void GraphicsBackend::shutdown()
	{

	}

	const RendererFeatures& GraphicsBackend::features()
	{
		static const RendererFeatures features { false, true, 4096 };
		return features;
	}

	void GraphicsBackend::frame() {}
	void GraphicsBackend::before_render() {}
	void GraphicsBackend::after_render() {}

	TextureRef GraphicsBackend::create_texture(int width, int height, TextureFilter filter, TextureWrap wrap_x, TextureWrap wrap_y, TextureFormat format)
	{
		return TextureRef(new Dummy_Texture(width, height, filter, wrap_x, wrap_y, format, false));
	}

	FrameBufferRef GraphicsBackend::create_framebuffer(int width, int height, const TextureFormat* attachments, int attachmentCount)
	{
		return FrameBufferRef(new Dummy_FrameBuffer(width, height, attachments, attachmentCount));
	}

	ShaderRef GraphicsBackend::create_shader(const ShaderData* data)
	{
		return ShaderRef(new Dummy_Shader(data));
	}

	MeshRef GraphicsBackend::create_mesh()
	{
		return MeshRef(new Dummy_Mesh());
	}

	void GraphicsBackend::render(const RenderPass& pass)
	{

	}

	void GraphicsBackend::clear_backbuffer(Color color)
	{

	}
}

#endif // !(defined(BLAH_USE_OPENGL) || defined(BLAH_USE_D3D11))
