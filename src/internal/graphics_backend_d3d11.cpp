#ifdef BLAH_USE_D3D11

// TODO:
// Note the D3D11 Implementation is still a work-in-progress

#include "../internal/graphics_backend.h"
#include "../internal/platform_backend.h"
#include <blah/core/log.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

namespace Blah
{
	class D3D11_Shader;

	struct D3D11
	{
		// main resources
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		IDXGISwapChain* swap_chain = nullptr;
		ID3D11RenderTargetView* backbuffer = nullptr;

		// supported renderer features
		RendererFeatures features;

		// last backbuffer size
		Point last_size;

		struct StoredInputLayout
		{
			uint32_t shader_hash;
			VertexFormat format;
			ID3D11InputLayout* layout;
		};

		struct StoredBlendState
		{
			BlendMode blend;
			ID3D11BlendState* state;
		};

		struct StoredRasterizer
		{
			Cull cull;
			bool has_scissor;
			ID3D11RasterizerState* state;
		};

		struct StoredSampler
		{
			TextureSampler sampler;
			ID3D11SamplerState* state;
		};

		struct StoredDepthStencil
		{
			Compare depth;
			ID3D11DepthStencilState* state;
		};

		Vector<StoredInputLayout> layout_cache;
		Vector<StoredBlendState> blend_cache;
		Vector<StoredRasterizer> rasterizer_cache;
		Vector<StoredSampler> sampler_cache;
		Vector<StoredDepthStencil> depthstencil_cache;

		ID3D11InputLayout* get_layout(D3D11_Shader* shader, const VertexFormat& format);
		ID3D11BlendState* get_blend(const BlendMode& blend);
		ID3D11RasterizerState* get_rasterizer(const RenderPass& pass);
		ID3D11SamplerState* get_sampler(const TextureSampler& sampler);
		ID3D11DepthStencilState* get_depthstencil(const RenderPass& pass);
	};

	// D3D11 State
	D3D11 state;

	// Utility Methods
	D3D11_BLEND_OP blend_op(BlendOp op);
	D3D11_BLEND blend_factor(BlendFactor factor);
	bool reflect_uniforms(Vector<UniformInfo>& append_uniforms_to, Vector<ID3D11Buffer*>& append_buffers_to, ID3DBlob* shader, ShaderType shader_type);
	void apply_uniforms(D3D11_Shader* shader, const MaterialRef& material, ShaderType type);

	// ~ BEGIN IMPLEMENTATION ~

	class D3D11_Texture : public Texture
	{
	private:
		int m_width;
		int m_height;
		TextureFormat m_format;
		DXGI_FORMAT m_dxgi_format;
		bool m_is_framebuffer;
		int m_size;

	public:
		ID3D11Texture2D* texture = nullptr;
		ID3D11Texture2D* staging = nullptr;
		ID3D11ShaderResourceView* view = nullptr;

		D3D11_Texture(int width, int height, TextureFormat format, bool is_framebuffer)
		{
			m_width = width;
			m_height = height;
			m_format = format;
			m_is_framebuffer = is_framebuffer;
			m_size = 0;

			D3D11_TEXTURE2D_DESC desc = { 0 };
			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			if (is_framebuffer)
				desc.BindFlags |= D3D11_BIND_RENDER_TARGET;

			switch (format)
			{
			case TextureFormat::R:
				desc.Format = DXGI_FORMAT_R8_UNORM;
				m_size = width * height;
				break;
			case TextureFormat::RG:
				desc.Format = DXGI_FORMAT_R8G8_UNORM;
				m_size = width * height * 2;
				break;
			case TextureFormat::RGBA:
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				m_size = width * height * 4;
				break;
			case TextureFormat::DepthStencil:
				desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				m_size = width * height * 4;
				break;
			}

			m_dxgi_format = desc.Format;

			auto hr = state.device->CreateTexture2D(&desc, NULL, &texture);
			if (!SUCCEEDED(hr))
			{
				if (texture)
					texture->Release();
				texture = nullptr;
				return;
			}

			hr = state.device->CreateShaderResourceView(texture, NULL, &view);
			if (!SUCCEEDED(hr))
			{
				texture->Release();
				texture = nullptr;
			}
		}

		~D3D11_Texture()
		{
			if (texture)
				texture->Release();
			if (staging)
				staging->Release();
			if (view)
				view->Release();
			staging = nullptr;
			texture = nullptr;
			view = nullptr;
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
			// bounds
			D3D11_BOX box;
			box.left = 0;
			box.right = m_width;
			box.top = 0;
			box.bottom = m_height;
			box.front = 0;
			box.back = 1;

			// set data
			state.context->UpdateSubresource(
				texture,
				0,
				&box,
				data,
				m_size / m_height,
				0);
		}

		virtual void get_data(unsigned char* data) override
		{
			HRESULT hr;

			// bounds
			D3D11_BOX box;
			box.left = 0;
			box.right = m_width;
			box.top = 0;
			box.bottom = m_height;
			box.front = 0;
			box.back = 1;

			// create staging texture
			if (!staging)
			{
				D3D11_TEXTURE2D_DESC desc;
				desc.Width = m_width;
				desc.Height = m_height;
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.Format = m_dxgi_format;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.Usage = D3D11_USAGE_STAGING;
				desc.BindFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				desc.MiscFlags = 0;

				hr = state.device->CreateTexture2D(&desc, NULL, &staging);
				if (!SUCCEEDED(hr))
				{
					BLAH_ERROR("Failed to create staging texture to get data");
					return;
				}
			}

			// copy data to staging texture
			state.context->CopySubresourceRegion(
				staging, 0,
				0, 0, 0,
				texture, 0,
				&box);

			// get data
			D3D11_MAPPED_SUBRESOURCE map;
			hr = state.context->Map(staging, 0, D3D11_MAP_READ, 0, &map);

			if (!SUCCEEDED(hr))
			{
				BLAH_ERROR("Failed to get texture data");
				return;
			}

			memcpy(data, map.pData, m_size);
			state.context->Unmap(staging, 0);
		}

		virtual bool is_framebuffer() const override
		{
			return m_is_framebuffer;
		}

	};

	class D3D11_FrameBuffer : public FrameBuffer
	{
	private:
		Attachments m_attachments;

	public:
		StackVector<ID3D11RenderTargetView*, Attachments::MaxCapacity - 1> color_views;
		ID3D11DepthStencilView* depth_view = nullptr;

		D3D11_FrameBuffer(int width, int height, const TextureFormat* attachments, int attachment_count)
		{
			for (int i = 0; i < attachment_count; i++)
			{
				auto tex = new D3D11_Texture(width, height, attachments[i], true);

				m_attachments.push_back(TextureRef(tex));

				if (attachments[i] == TextureFormat::DepthStencil)
				{
					state.device->CreateDepthStencilView(tex->texture, nullptr, &depth_view);
				}
				else
				{
					ID3D11RenderTargetView* view = nullptr;
					state.device->CreateRenderTargetView(tex->texture, nullptr, &view);
					color_views.push_back(view);
				}
			}
		}

		~D3D11_FrameBuffer()
		{
			for (auto& it : color_views)
				it->Release();
			color_views.clear();
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
			float col[4] = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };

			for (int i = 0; i < m_attachments.size(); i++)
				state.context->ClearRenderTargetView(color_views[i], col);
		}
	};

	class D3D11_Shader : public Shader
	{
	public:
		ID3D11VertexShader* vertex = nullptr;
		ID3D11PixelShader* fragment = nullptr;
		ID3DBlob* vertex_blob = nullptr;
		ID3DBlob* fragment_blob = nullptr;
		Vector<ID3D11Buffer*> vertex_uniform_buffers;
		Vector<ID3D11Buffer*> fragment_uniform_buffers;
		Vector<Vector<float>> vertex_uniform_values;
		Vector<Vector<float>> fragment_uniform_values;
		StackVector<ShaderData::HLSL_Attribute, 16> attributes;
		Vector<UniformInfo> uniform_list;
		uint32_t hash = 0;
		bool valid = false;

		D3D11_Shader(const ShaderData* data)
		{
			UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
			ID3DBlob* error_blob = nullptr;
			HRESULT hr;

			// compile vertex shader
			{
				hr = D3DCompile(
					data->vertex.cstr(),
					data->vertex.length(),
					nullptr,
					nullptr,
					nullptr,
					"vs_main",
					"vs_5_0",
					flags,
					0,
					&vertex_blob,
					&error_blob);

				if (FAILED(hr))
				{
					Log::error("%s", (char*)error_blob->GetBufferPointer());
					error_blob->Release();
					return;
				}
			}

			// compile fragment shader
			{
				hr = D3DCompile(
					data->fragment.cstr(),
					data->fragment.length(),
					nullptr,
					nullptr,
					nullptr,
					"ps_main",
					"ps_5_0",
					flags,
					0,
					&fragment_blob,
					&error_blob);

				if (FAILED(hr))
				{
					Log::error("%s", (char*)error_blob->GetBufferPointer());
					error_blob->Release();
					return;
				}
			}

			// create vertex shader
			{
				hr = state.device->CreateVertexShader(
					vertex_blob->GetBufferPointer(),
					vertex_blob->GetBufferSize(),
					NULL,
					&vertex);

				if (!SUCCEEDED(hr))
					return;
			}

			// create fragment shader
			{
				hr = state.device->CreatePixelShader(
					fragment_blob->GetBufferPointer(),
					fragment_blob->GetBufferSize(),
					NULL,
					&fragment);

				if (!SUCCEEDED(hr))
					return;
			}

			// get uniforms
			reflect_uniforms(uniform_list, vertex_uniform_buffers, vertex_blob, ShaderType::Vertex);
			reflect_uniforms(uniform_list, fragment_uniform_buffers, fragment_blob, ShaderType::Fragment);

			// combine uniforms that were in both
			for (int i = 0; i < uniform_list.size(); i++)
			{
				for (int j = i + 1; j < uniform_list.size(); j++)
				{
					if (strcmp(uniform_list[i].name, uniform_list[j].name) == 0)
					{
						if (uniform_list[i].type == uniform_list[j].type)
						{
							uniform_list[i].shader = (ShaderType)((int)uniform_list[i].shader | (int)uniform_list[j].shader);
							uniform_list.erase(j);
							j--;
						}
					}
				}
			}

			// create CPU uniform buffers, so we don't need to create them during rendering
			vertex_uniform_values.expand(vertex_uniform_buffers.size());
			fragment_uniform_values.expand(fragment_uniform_buffers.size());

			// copy HLSL attributes
			attributes = data->hlsl_attributes;

			// store hash
			hash = 5381;
			for (auto& it : attributes)
			{
				for (int i = 0, n = strlen(it.semantic_name); i < n; i++)
					hash = ((hash << 5) + hash) + it.semantic_name[i];
				hash = it.semantic_index << 5 + hash;
			}

			// Shader is ready for use!
			valid = true;
		}

		~D3D11_Shader()
		{
			if (vertex)
				vertex->Release();
			if (vertex_blob)
				vertex_blob->Release();
			if (fragment)
				fragment->Release();
			if (fragment_blob)
				fragment_blob->Release();

			for (auto& it : vertex_uniform_buffers)
				it->Release();
			for (auto& it : fragment_uniform_buffers)
				it->Release();

			vertex = nullptr;
			vertex_blob = nullptr;
			fragment = nullptr;
			fragment_blob = nullptr;
		}

		virtual Vector<UniformInfo>& uniforms() override
		{
			return uniform_list;
		}

		virtual const Vector<UniformInfo>& uniforms() const override
		{
			return uniform_list;
		}
	};

	class D3D11_Mesh : public Mesh
	{
	private:
		int64_t m_vertex_count = 0;
		int64_t m_vertex_capacity = 0;
		int64_t m_index_count = 0;
		int64_t m_index_capacity = 0;

	public:
		ID3D11Buffer* vertex_buffer = nullptr;
		VertexFormat vertex_format;
		ID3D11Buffer* index_buffer = nullptr;
		IndexFormat index_format = IndexFormat::UInt16;
		int index_stride = 0;

		D3D11_Mesh()
		{

		}

		~D3D11_Mesh()
		{
			if (vertex_buffer)
				vertex_buffer->Release();
			if (index_buffer)
				index_buffer->Release();
		}

		virtual void index_data(IndexFormat format, const void* indices, int64_t count) override
		{
			m_index_count = count;

			if (index_format != format || !index_buffer || m_index_count > m_index_capacity)
			{
				index_stride = 0;
				index_format = format;
				m_index_capacity = max(m_index_capacity, m_index_count);

				switch (format)
				{
				case IndexFormat::UInt16: index_stride = sizeof(int16_t); break;
				case IndexFormat::UInt32: index_stride = sizeof(int32_t); break;
				}

				if (m_index_capacity > 0 && indices)
				{
					// release existing buffer
					if (index_buffer)
						index_buffer->Release();
					index_buffer = nullptr;
				
					// buffer description
					D3D11_BUFFER_DESC desc = { 0 };
					desc.ByteWidth = index_stride * m_index_capacity;
					desc.Usage = D3D11_USAGE_DYNAMIC;
					desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
					desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

					// buffer data
					D3D11_SUBRESOURCE_DATA data = { 0 };
					data.pSysMem = indices;

					// create
					auto hr = state.device->CreateBuffer(&desc, &data, &index_buffer);
					BLAH_ASSERT(SUCCEEDED(hr), "Failed to update Index Data");
				}
			}
			else if (indices)
			{
				D3D11_MAPPED_SUBRESOURCE map;

				auto hr = state.context->Map(index_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
				BLAH_ASSERT(SUCCEEDED(hr), "Failed to update Index Data");

				if (SUCCEEDED(hr))
				{
					memcpy(map.pData, indices, index_stride * count);
					state.context->Unmap(index_buffer, 0);
				}
			}
		}

		virtual void vertex_data(const VertexFormat& format, const void* vertices, int64_t count) override
		{
			m_vertex_count = count;

			// recreate buffer if we've changed
			if (vertex_format.stride != format.stride || !vertex_buffer || m_vertex_count > m_vertex_capacity)
			{
				m_vertex_capacity = max(m_vertex_capacity, m_vertex_count);
				vertex_format = format;

				// discard old buffer
				if (vertex_buffer)
					vertex_buffer->Release();
				vertex_buffer = nullptr;

				if (m_vertex_capacity > 0 && vertices)
				{
					// buffer description
					D3D11_BUFFER_DESC desc = { 0 };
					desc.ByteWidth = format.stride * m_vertex_capacity;
					desc.Usage = D3D11_USAGE_DYNAMIC;
					desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

					// buffer data
					D3D11_SUBRESOURCE_DATA data = { 0 };
					data.pSysMem = vertices;

					// create
					auto hr = state.device->CreateBuffer(&desc, &data, &vertex_buffer);
					BLAH_ASSERT(SUCCEEDED(hr), "Failed to update Vertex Data");
				}
			}
			// otherwise just update it
			else if (vertices)
			{
				D3D11_MAPPED_SUBRESOURCE map;
				auto hr = state.context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
				BLAH_ASSERT(SUCCEEDED(hr), "Failed to update Vertex Data");

				if (SUCCEEDED(hr))
				{
					memcpy(map.pData, vertices, vertex_format.stride * count);
					state.context->Unmap(vertex_buffer, 0);
				}
			}
		}

		virtual void instance_data(const VertexFormat& format, const void* instances, int64_t count) override
		{

		}

		virtual int64_t index_count() const override
		{
			return m_index_count;
		}

		virtual int64_t vertex_count() const override
		{
			return m_vertex_count;
		}

		virtual int64_t instance_count() const override
		{
			return 0;
		}
	};

	bool GraphicsBackend::init()
	{
		state = D3D11();
		state.last_size = Point(App::draw_width(), App::draw_height());

		// Define Swap Chain
		DXGI_SWAP_CHAIN_DESC desc = { 0 };
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;
		desc.OutputWindow = (HWND)PlatformBackend::d3d11_get_hwnd();
		//desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Windowed = true;

		// Creation Flags
		UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(DEBUG) || defined(_DEBUG)
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// Create D3D device & context & swap cahin
		D3D_FEATURE_LEVEL feature_level;
		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			flags,
			NULL,
			0,
			D3D11_SDK_VERSION,
			&desc,
			&state.swap_chain,
			&state.device,
			&feature_level,
			&state.context);

		// Exit out if it's not OK
		if (!SUCCEEDED(hr) || !state.swap_chain || !state.device || !state.context)
			return false;

		// Get the backbuffer
		ID3D11Texture2D* frame_buffer = nullptr;
		state.swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&frame_buffer);
		if (frame_buffer)
		{
			state.device->CreateRenderTargetView(frame_buffer, nullptr, &state.backbuffer);
			frame_buffer->Release();
		}

		// Store Features
		state.features.instancing = true;
		state.features.max_texture_size = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
		state.features.origin_bottom_left = false;

		// Print Driver Info
		{
			IDXGIDevice* dxgi_device;
			IDXGIAdapter* dxgi_adapter;
			DXGI_ADAPTER_DESC adapter_desc;

			hr = state.device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device);

			if (SUCCEEDED(hr))
			{
				dxgi_device->GetAdapter(&dxgi_adapter);
				dxgi_adapter->GetDesc(&adapter_desc);

				Log::print("D3D11 %ls", adapter_desc.Description);
			}
			else
				Log::print("D3D11");
		}

		return true;
	}

	Renderer GraphicsBackend::renderer()
	{
		return Renderer::D3D11;
	}

	void GraphicsBackend::shutdown()
	{
		// release cached objects
		for (auto& it : state.blend_cache)
			it.state->Release();
		for (auto& it : state.depthstencil_cache)
			it.state->Release();
		for (auto& it : state.layout_cache)
			it.layout->Release();
		for (auto& it : state.rasterizer_cache)
			it.state->Release();
		for (auto& it : state.sampler_cache)
			it.state->Release();

		// TODO:
		// Do we need to release live resources? ex. Texture's that
		// haven't been released by shutdown will still exist...

		// release main devices
		state.swap_chain->Release();
		state.context->Release();
		state.device->Release();

		// reset state
		state = D3D11();
	}

	const RendererFeatures& GraphicsBackend::features()
	{
		return state.features;
	}

	void GraphicsBackend::frame()
	{
	}

	void GraphicsBackend::before_render()
	{
		HRESULT hr;

		auto next_size = Point(App::draw_width(), App::draw_height());
		if (state.last_size != next_size)
		{
			// release old buffer
			if (state.backbuffer)
				state.backbuffer->Release();

			// perform resize
			hr = state.swap_chain->ResizeBuffers(0, next_size.x, next_size.y, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
			BLAH_ASSERT(SUCCEEDED(hr), "Failed to update Backbuffer on Resize");
			state.last_size = next_size;

			// get the new buffer
			ID3D11Texture2D* frame_buffer = nullptr;
			hr = state.swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&frame_buffer);
			if (SUCCEEDED(hr) && frame_buffer)
			{
				hr = state.device->CreateRenderTargetView(frame_buffer, nullptr, &state.backbuffer);
				BLAH_ASSERT(SUCCEEDED(hr), "Failed to update Backbuffer on Resize");
				frame_buffer->Release();
			}
		}
	}

	void GraphicsBackend::after_render()
	{
		auto hr = state.swap_chain->Present(1, 0);
		BLAH_ASSERT(SUCCEEDED(hr), "Failed to Present swap chain");
	}

	TextureRef GraphicsBackend::create_texture(int width, int height, TextureFormat format)
	{
		auto result = new D3D11_Texture(width, height, format, false);

		if (result->texture)
			return TextureRef(result);

		delete result;
		return TextureRef();
	}

	FrameBufferRef GraphicsBackend::create_framebuffer(int width, int height, const TextureFormat* attachments, int attachment_count)
	{
		return FrameBufferRef(new D3D11_FrameBuffer(width, height, attachments, attachment_count));
	}

	ShaderRef GraphicsBackend::create_shader(const ShaderData* data)
	{
		auto result = new D3D11_Shader(data);
		if (result->valid)
			return ShaderRef(result);

		delete result;
		return ShaderRef();
	}

	MeshRef GraphicsBackend::create_mesh()
	{
		return MeshRef(new D3D11_Mesh());
	}

	void GraphicsBackend::render(const RenderPass& pass)
	{
		auto ctx = state.context;
		auto mesh = (D3D11_Mesh*)pass.mesh.get();
		auto shader = (D3D11_Shader*)(pass.material->shader().get());

		// OM
		{
			// Set the Target
			if (pass.target == App::backbuffer || !pass.target)
			{
				ctx->OMSetRenderTargets(1, &state.backbuffer, nullptr);
			}
			else
			{
				auto target = (D3D11_FrameBuffer*)(pass.target.get());
				ctx->OMSetRenderTargets(target->color_views.size(), target->color_views.begin(), target->depth_view);
			}

			// Depth
			{
				auto depthstencil = state.get_depthstencil(pass);
				if (depthstencil)
					ctx->OMSetDepthStencilState(depthstencil, 0);
			}

			// Blend Mode
			{
				auto blend = state.get_blend(pass.blend);
				if (blend)
				{
					auto color = Color::from_rgba(pass.blend.rgba);
					float factor[4]{ color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
					auto mask = 0xffffffff;
					ctx->OMSetBlendState(blend, factor, mask);
				}
				else
				{
					// if we failed to create a blend mode for some reason
					ctx->OMSetBlendState(nullptr, nullptr, 0);
				}
			}
		}

		// IA
		{
			// We draw triangles
			ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// Assign Layout
			auto layout = state.get_layout(shader, mesh->vertex_format);
			ctx->IASetInputLayout(layout);

			// Assign Vertex Buffer
			{
				UINT stride = mesh->vertex_format.stride;
				UINT offset = 0;

				ctx->IASetVertexBuffers(
					0,
					1,
					&mesh->vertex_buffer,
					&stride,
					&offset);

				D3D11_BUFFER_DESC desc;
				mesh->vertex_buffer->GetDesc(&desc);
			}

			// Assign Index Buffer
			{
				DXGI_FORMAT format = DXGI_FORMAT_R16_UINT;

				switch (mesh->index_format)
				{
				case IndexFormat::UInt16: format = DXGI_FORMAT_R16_UINT; break;
				case IndexFormat::UInt32: format = DXGI_FORMAT_R32_UINT; break;
				}

				ctx->IASetIndexBuffer(mesh->index_buffer, format, 0);
			}
		}

		// VS
		{
			apply_uniforms(shader, pass.material, ShaderType::Vertex);
			ctx->VSSetShader(shader->vertex, nullptr, 0);
			ctx->VSSetConstantBuffers(0, shader->vertex_uniform_buffers.size(), shader->vertex_uniform_buffers.begin());
		}

		// PS
		{
			apply_uniforms(shader, pass.material, ShaderType::Fragment);
			ctx->PSSetShader(shader->fragment, nullptr, 0);
			ctx->PSSetConstantBuffers(0, shader->fragment_uniform_buffers.size(), shader->fragment_uniform_buffers.begin());

			// Fragment Shader Textures
			auto& textures = pass.material->textures();
			for (int i = 0; i < textures.size(); i++)
			{
				if (textures[i])
				{
					// Assign the Texture
					auto view = ((D3D11_Texture*)textures[i].get())->view;
					ctx->PSSetShaderResources(i, 1, &view);
				}
			}

			// Fragment Shader Samplers
			auto& samplers = pass.material->samplers();
			for (int i = 0; i < samplers.size(); i++)
			{
				auto sampler = state.get_sampler(samplers[i]);
				if (sampler)
					ctx->PSSetSamplers(i, 1, &sampler);
			}
		}

		// RS
		{
			// Set the Viewport
			{
				D3D11_VIEWPORT viewport = {
					pass.viewport.x, pass.viewport.y,
					pass.viewport.w, pass.viewport.h,
					0.0f, 1.0f };

				ctx->RSSetViewports(1, &viewport);
			}

			// Scissor Rect
			if (pass.has_scissor)
			{
				D3D11_RECT scissor = {
					(int)pass.scissor.x, (int)pass.scissor.y,
					(int)pass.scissor.x + (int)pass.scissor.w, (int)pass.scissor.y + (int)pass.scissor.h };

				ctx->RSSetScissorRects(1, &scissor);
			}
			else
			{
				ctx->RSSetScissorRects(0, nullptr);
			}

			// Rasterizer
			{
				auto rasterizer = state.get_rasterizer(pass);
				if (rasterizer)
					ctx->RSSetState(rasterizer);
			}
		}

		// Draw
		{
			if (mesh->instance_count() <= 0)
			{
				ctx->DrawIndexed(pass.index_count, pass.index_start, 0);
			}
			else
			{
				// TODO:
				// Draw Instanced data
				BLAH_ASSERT(false, "Instanced Drawing not implemented yet");
			}
		}

		// UnBind Shader Resources
		{
			auto& textures = pass.material->textures();
			ID3D11ShaderResourceView* view = nullptr;
			for (int i = 0; i < textures.size(); i++)
				ctx->PSSetShaderResources(i, 1, &view);
		}
	}

	void GraphicsBackend::clear_backbuffer(Color color)
	{
		float clear[4] = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
		state.context->ClearRenderTargetView(state.backbuffer, clear);
	}

	// Utility Methods

	D3D11_BLEND_OP blend_op(BlendOp op)
	{
		switch (op)
		{
		case BlendOp::Add: return D3D11_BLEND_OP_ADD;
		case BlendOp::Subtract: return D3D11_BLEND_OP_SUBTRACT;
		case BlendOp::ReverseSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
		case BlendOp::Min: return D3D11_BLEND_OP_MIN;
		case BlendOp::Max: return D3D11_BLEND_OP_MAX;
		}

		return D3D11_BLEND_OP_ADD;
	}

	D3D11_BLEND blend_factor(BlendFactor factor)
	{
		switch (factor)
		{
		case BlendFactor::Zero: return D3D11_BLEND_ZERO;
		case BlendFactor::One: return D3D11_BLEND_ONE;
		case BlendFactor::SrcColor: return D3D11_BLEND_SRC_COLOR;
		case BlendFactor::OneMinusSrcColor: return D3D11_BLEND_INV_SRC_COLOR;
		case BlendFactor::DstColor: return D3D11_BLEND_DEST_COLOR;
		case BlendFactor::OneMinusDstColor: return D3D11_BLEND_INV_DEST_COLOR;
		case BlendFactor::SrcAlpha: return D3D11_BLEND_SRC_ALPHA;
		case BlendFactor::OneMinusSrcAlpha: return D3D11_BLEND_INV_SRC_ALPHA;
		case BlendFactor::DstAlpha: return D3D11_BLEND_DEST_ALPHA;
		case BlendFactor::OneMinusDstAlpha: return D3D11_BLEND_INV_DEST_ALPHA;
		case BlendFactor::ConstantColor: return D3D11_BLEND_BLEND_FACTOR;
		case BlendFactor::OneMinusConstantColor: return D3D11_BLEND_INV_BLEND_FACTOR;
		case BlendFactor::ConstantAlpha: return D3D11_BLEND_BLEND_FACTOR;
		case BlendFactor::OneMinusConstantAlpha: return D3D11_BLEND_INV_BLEND_FACTOR;
		case BlendFactor::SrcAlphaSaturate: return D3D11_BLEND_SRC_ALPHA_SAT;
		case BlendFactor::Src1Color: return D3D11_BLEND_SRC1_COLOR;
		case BlendFactor::OneMinusSrc1Color: return D3D11_BLEND_INV_SRC1_COLOR;
		case BlendFactor::Src1Alpha: return D3D11_BLEND_SRC1_ALPHA;
		case BlendFactor::OneMinusSrc1Alpha: return D3D11_BLEND_INV_SRC1_ALPHA;
		}

		return D3D11_BLEND_ZERO;
	}

	bool reflect_uniforms(Vector<UniformInfo>& append_uniforms_to, Vector<ID3D11Buffer*>& append_buffers_to, ID3DBlob* shader, ShaderType shader_type)
	{
		ID3D11ShaderReflection* reflector = nullptr;
		D3DReflect(shader->GetBufferPointer(), shader->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);

		D3D11_SHADER_DESC shader_desc;
		reflector->GetDesc(&shader_desc);

		for (int i = 0; i < shader_desc.BoundResources; i++)
		{
			D3D11_SHADER_INPUT_BIND_DESC desc;
			reflector->GetResourceBindingDesc(i, &desc);

			if (desc.Type == D3D_SIT_TEXTURE && desc.Dimension == D3D_SRV_DIMENSION_TEXTURE2D)
			{
				auto uniform = append_uniforms_to.expand();
				uniform->name = desc.Name;
				uniform->shader = shader_type;
				uniform->buffer_index = 0;
				uniform->array_length = max(1, desc.BindCount);
				uniform->type = UniformType::Texture2D;
			}
			else if (desc.Type == D3D_SIT_SAMPLER)
			{
				auto uniform = append_uniforms_to.expand();
				uniform->name = desc.Name;
				uniform->shader = shader_type;
				uniform->buffer_index = 0;
				uniform->array_length = max(1, desc.BindCount);
				uniform->type = UniformType::Sampler2D;
			}
		}

		for (int i = 0; i < shader_desc.ConstantBuffers; i++)
		{
			D3D11_SHADER_BUFFER_DESC desc;

			auto cb = reflector->GetConstantBufferByIndex(i);
			cb->GetDesc(&desc);

			// create the constant buffer for assigning data later
			{
				D3D11_BUFFER_DESC buffer_desc = {};
				buffer_desc.ByteWidth = desc.Size;
				buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
				buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

				ID3D11Buffer* buffer;
				state.device->CreateBuffer(&buffer_desc, nullptr, &buffer);
				append_buffers_to.push_back(buffer);
			}

			// get the uniforms
			for (int j = 0; j < desc.Variables; j++)
			{
				D3D11_SHADER_VARIABLE_DESC var_desc;
				D3D11_SHADER_TYPE_DESC type_desc;

				auto var = cb->GetVariableByIndex(j);
				var->GetDesc(&var_desc);

				auto type = var->GetType();
				type->GetDesc(&type_desc);

				auto uniform = append_uniforms_to.expand();
				uniform->name = var_desc.Name;
				uniform->shader = shader_type;
				uniform->buffer_index = i;
				uniform->array_length = max(1, type_desc.Elements);
				uniform->type = UniformType::None;

				if (type_desc.Type == D3D_SVT_FLOAT)
				{
					if (type_desc.Rows == 1)
					{
						if (type_desc.Columns == 1)
							uniform->type = UniformType::Float;
						else if (type_desc.Columns == 2)
							uniform->type = UniformType::Float2;
						else if (type_desc.Columns == 3)
							uniform->type = UniformType::Float3;
						else if (type_desc.Columns == 4)
							uniform->type = UniformType::Float4;
					}
					else if (type_desc.Rows == 2 && type_desc.Columns == 3)
					{
						uniform->type = UniformType::Mat3x2;
					}
					else if (type_desc.Rows == 4 && type_desc.Columns == 4)
					{
						uniform->type = UniformType::Mat4x4;
					}
				}
			}
		}

		return true;
	}

	void apply_uniforms(D3D11_Shader* shader, const MaterialRef& material, ShaderType type)
	{
		auto& buffers = (type == ShaderType::Vertex ? shader->vertex_uniform_buffers : shader->fragment_uniform_buffers);
		auto& values = (type == ShaderType::Vertex ? shader->vertex_uniform_values : shader->fragment_uniform_values);

		for (int i = 0; i < buffers.size(); i++)
		{
			// clear previous values
			values[i].clear();

			// build block
			const float* data = material->data();
			for (auto& it : shader->uniforms())
			{
				if (it.type == UniformType::None ||
					it.type == UniformType::Texture2D ||
					it.type == UniformType::Sampler2D)
					continue;

				int size = 0;
				switch (it.type)
				{
				case UniformType::Float: size = 1; break;
				case UniformType::Float2: size = 2; break;
				case UniformType::Float3: size = 3; break;
				case UniformType::Float4: size = 4; break;
				case UniformType::Mat3x2: size = 6; break;
				case UniformType::Mat4x4: size = 16; break;
				}

				int length = size * it.array_length;

				if (it.buffer_index == i && ((int)it.shader & (int)type) != 0)
				{
					auto start = values[i].expand(length);
					memcpy(start, data, sizeof(float) * length);
				}

				data += length;
			}

			// apply block
			if (buffers[i])
			{
				D3D11_MAPPED_SUBRESOURCE map;
				state.context->Map(buffers[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
				memcpy(map.pData, values[i].begin(), values[i].size() * sizeof(float));
				state.context->Unmap(buffers[i], 0);
			}
		}
	}

	ID3D11InputLayout* D3D11::get_layout(D3D11_Shader* shader, const VertexFormat& format)
	{
		// find existing
		for (auto& it : layout_cache)
		{
			if (it.shader_hash == shader->hash && it.format.stride == format.stride && it.format.attributes.size() == format.attributes.size())
			{
				bool same_format = true;
				for (int n = 0; same_format && n < format.attributes.size(); n++)
					if (it.format.attributes[n].index != format.attributes[n].index || 
						it.format.attributes[n].type != format.attributes[n].type ||
						it.format.attributes[n].normalized != format.attributes[n].normalized)
						same_format = false;

				if (same_format)
					return it.layout;
			}
		}

		// create a new one
		Vector<D3D11_INPUT_ELEMENT_DESC> desc;
		for (int i = 0; i < shader->attributes.size(); i++)
		{
			auto it = desc.expand();
			it->SemanticName = shader->attributes[i].semantic_name;
			it->SemanticIndex = shader->attributes[i].semantic_index;

			if (!format.attributes[i].normalized)
			{
				switch (format.attributes[i].type)
				{
				case VertexType::Float: it->Format = DXGI_FORMAT_R32_FLOAT;  break;
				case VertexType::Float2: it->Format = DXGI_FORMAT_R32G32_FLOAT; break;
				case VertexType::Float3: it->Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
				case VertexType::Float4: it->Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
				case VertexType::Byte4: it->Format = DXGI_FORMAT_R8G8B8A8_SINT; break;
				case VertexType::UByte4: it->Format = DXGI_FORMAT_R8G8B8A8_UINT; break;
				case VertexType::Short2: it->Format = DXGI_FORMAT_R16G16_SINT; break;
				case VertexType::UShort2: it->Format = DXGI_FORMAT_R16G16_UINT; break;
				case VertexType::Short4: it->Format = DXGI_FORMAT_R16G16B16A16_SINT; break;
				case VertexType::UShort4: it->Format = DXGI_FORMAT_R16G16B16A16_UINT; break;
				}
			}
			else
			{
				switch (format.attributes[i].type)
				{
				case VertexType::Float: it->Format = DXGI_FORMAT_R32_FLOAT;  break;
				case VertexType::Float2: it->Format = DXGI_FORMAT_R32G32_FLOAT; break;
				case VertexType::Float3: it->Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
				case VertexType::Float4: it->Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
				case VertexType::Byte4: it->Format = DXGI_FORMAT_R8G8B8A8_SNORM; break;
				case VertexType::UByte4: it->Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
				case VertexType::Short2: it->Format = DXGI_FORMAT_R16G16_SNORM; break;
				case VertexType::UShort2: it->Format = DXGI_FORMAT_R16G16_UNORM; break;
				case VertexType::Short4: it->Format = DXGI_FORMAT_R16G16B16A16_SNORM; break;
				case VertexType::UShort4: it->Format = DXGI_FORMAT_R16G16B16A16_UNORM; break;
				}
			}

			it->InputSlot = 0;
			it->AlignedByteOffset = (i == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT);
			it->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			it->InstanceDataStepRate = 0;
		}

		ID3D11InputLayout* layout = nullptr;

		auto hr = device->CreateInputLayout(
			desc.begin(),
			desc.size(),
			shader->vertex_blob->GetBufferPointer(),
			shader->vertex_blob->GetBufferSize(),
			&layout);

		if (SUCCEEDED(hr))
		{
			auto entry = layout_cache.expand();
			entry->shader_hash = shader->hash;
			entry->format = format;
			entry->layout = layout;
			return layout;
		}

		return nullptr;
	}

	ID3D11BlendState* D3D11::get_blend(const BlendMode& blend)
	{
		for (auto& it : blend_cache)
			if (it.blend == blend)
				return it.state;

		D3D11_BLEND_DESC desc = { 0 };
		desc.AlphaToCoverageEnable = 0;
		desc.IndependentBlendEnable = 0;

		desc.RenderTarget[0].BlendEnable = !(
			blend.color_src == BlendFactor::One && blend.color_dst == BlendFactor::Zero &&
			blend.alpha_src == BlendFactor::One && blend.alpha_dst == BlendFactor::Zero
			);

		desc.RenderTarget[0].RenderTargetWriteMask = 0;
		if (((int)blend.mask & (int)BlendMask::Red) == (int)BlendMask::Red)
			desc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_RED;
		if (((int)blend.mask & (int)BlendMask::Green) == (int)BlendMask::Green)
			desc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_GREEN;
		if (((int)blend.mask & (int)BlendMask::Blue) == (int)BlendMask::Blue)
			desc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_BLUE;
		if (((int)blend.mask & (int)BlendMask::Alpha) == (int)BlendMask::Alpha)
			desc.RenderTarget[0].RenderTargetWriteMask |= D3D11_COLOR_WRITE_ENABLE_ALPHA;

		if (desc.RenderTarget[0].BlendEnable)
		{
			desc.RenderTarget[0].BlendOp = blend_op(blend.color_op);
			desc.RenderTarget[0].SrcBlend = blend_factor(blend.color_src);
			desc.RenderTarget[0].DestBlend = blend_factor(blend.color_dst);

			desc.RenderTarget[0].BlendOpAlpha = blend_op(blend.alpha_op);
			desc.RenderTarget[0].SrcBlendAlpha = blend_factor(blend.alpha_src);
			desc.RenderTarget[0].DestBlendAlpha = blend_factor(blend.alpha_dst);
		}

		for (int i = 1; i < 8; i ++)
			desc.RenderTarget[i] = desc.RenderTarget[0];

		ID3D11BlendState* blend_state = nullptr;
		auto hr = state.device->CreateBlendState(&desc, &blend_state);

		if (SUCCEEDED(hr))
		{
			auto entry = blend_cache.expand();
			entry->blend = blend;
			entry->state = blend_state;
			return blend_state;
		}

		return nullptr;
	}

	ID3D11SamplerState* D3D11::get_sampler(const TextureSampler& sampler)
	{
		for (auto& it : sampler_cache)
			if (it.sampler == sampler)
				return it.state;

		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

		switch (sampler.filter)
		{
		case TextureFilter::Nearest: desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; break;
		case TextureFilter::Linear: desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
		}

		switch (sampler.wrap_x)
		{
		case TextureWrap::Clamp: desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP; break;
		case TextureWrap::Repeat: desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; break;
		}

		switch (sampler.wrap_y)
		{
		case TextureWrap::Clamp: desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP; break;
		case TextureWrap::Repeat: desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; break;
		}
		
		ID3D11SamplerState* result;
		auto hr = state.device->CreateSamplerState(&desc, &result);

		if (SUCCEEDED(hr))
		{
			auto entry = sampler_cache.expand();
			entry->sampler = sampler;
			entry->state = result;
			return result;
		}

		return nullptr;
	}

	ID3D11RasterizerState* D3D11::get_rasterizer(const RenderPass& pass)
	{
		for (auto& it : rasterizer_cache)
			if (it.cull == pass.cull && it.has_scissor == pass.has_scissor)
				return it.state;

		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;

		switch (pass.cull)
		{
		case Cull::None: desc.CullMode = D3D11_CULL_NONE; break;
		case Cull::Front: desc.CullMode = D3D11_CULL_FRONT; break;
		case Cull::Back: desc.CullMode = D3D11_CULL_BACK; break;
		}

		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = false;
		desc.ScissorEnable = pass.has_scissor;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;

		ID3D11RasterizerState* result;
		auto hr = state.device->CreateRasterizerState(&desc, &result);

		if (SUCCEEDED(hr))
		{
			auto entry = rasterizer_cache.expand();
			entry->cull = pass.cull;
			entry->has_scissor = pass.has_scissor;
			entry->state = result;
			return result;
		}

		return nullptr;
	}

	ID3D11DepthStencilState* D3D11::get_depthstencil(const RenderPass& pass)
	{
		for (auto& it : depthstencil_cache)
			if (it.depth == pass.depth)
				return it.state;

		D3D11_DEPTH_STENCIL_DESC desc = {};
		desc.DepthEnable = pass.depth != Compare::None;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_NEVER;

		switch (pass.depth)
		{
			case Compare::None: desc.DepthFunc = D3D11_COMPARISON_NEVER; break;
			case Compare::Always: desc.DepthFunc = D3D11_COMPARISON_ALWAYS; break;
			case Compare::Never: desc.DepthFunc = D3D11_COMPARISON_NEVER; break;
			case Compare::Less: desc.DepthFunc = D3D11_COMPARISON_LESS; break;
			case Compare::Equal: desc.DepthFunc = D3D11_COMPARISON_EQUAL; break;
			case Compare::LessOrEqual: desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; break;
			case Compare::Greater: desc.DepthFunc = D3D11_COMPARISON_GREATER; break;
			case Compare::NotEqual: desc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL; break;
			case Compare::GreatorOrEqual: desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL; break;
		}

		ID3D11DepthStencilState* result;
		auto hr = state.device->CreateDepthStencilState(&desc, &result);

		if (SUCCEEDED(hr))
		{
			auto entry = depthstencil_cache.expand();
			entry->depth = pass.depth;
			entry->state = result;
			return result;
		}

		return nullptr;
	}
}

#endif // BLAH_USE_D3D11
