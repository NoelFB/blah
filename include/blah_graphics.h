#pragma once
#include <blah_common.h>
#include <blah_vector.h>
#include <blah_stackvector.h>
#include <blah_string.h>
#include <blah_spatial.h>
#include <blah_image.h>

namespace Blah
{
	class Stream;

	class Shader;   using ShaderRef   = Ref<Shader>;
	class Texture;  using TextureRef  = Ref<Texture>;
	class Target;   using TargetRef   = Ref<Target>;
	class Mesh;     using MeshRef     = Ref<Mesh>;
	class Material; using MaterialRef = Ref<Material>;

	// Type of Renderer the Application is using
	enum class RendererType
	{
		None = -1,
		OpenGL,
		D3D11,
	};

	// Renderer Information
	struct RendererInfo
	{
		// The type of Renderer being used
		RendererType type = RendererType::None;

		// Whether Mesh Instancing is available
		bool instancing = false;

		// Whether the Texture origin is the bottom left.
		// This is true for OpenGL.
		bool origin_bottom_left = false;

		// Maximum Texture Size available
		int max_texture_size = 0;
	};

	// Depth comparison function to use during a draw call
	enum class Compare
	{
		None,
		Always,
		Never,
		Less,
		Equal,
		LessOrEqual,
		Greater,
		NotEqual,
		GreaterOrEqual
	};

	// Cull mode during a draw call
	enum class Cull
	{
		// No Culling enabled
		None = 0,

		// Cull front faces
		Front = 1,

		// Cull back faces
		Back = 2,
	};

	enum class BlendOp
	{
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max
	};

	enum class BlendFactor
	{
		Zero,
		One,
		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha,
		ConstantColor,
		OneMinusConstantColor,
		ConstantAlpha,
		OneMinusConstantAlpha,
		SrcAlphaSaturate,
		Src1Color,
		OneMinusSrc1Color,
		Src1Alpha,
		OneMinusSrc1Alpha
	};

	enum class BlendMask
	{
		None = 0,
		Red = 1,
		Green = 2,
		Blue = 4,
		Alpha = 8,
		RGB = Red | Green | Blue,
		RGBA = Red | Green | Blue | Alpha,
	};

	// BlendMode using for rendering
	struct BlendMode
	{
		// Normal is Premultipled Alpha
		// TODO: potentially rename normal to match that it is for Premultiplied Alpha?
		static const BlendMode Normal;
		static const BlendMode NonPremultiplied;
		static const BlendMode Subtract;
		static const BlendMode Additive;

		BlendOp color_op;
		BlendFactor color_src;
		BlendFactor color_dst;
		BlendOp alpha_op;
		BlendFactor alpha_src;
		BlendFactor alpha_dst;
		BlendMask mask;
		u32 rgba;

		BlendMode() = default;

		BlendMode(BlendOp op, BlendFactor src, BlendFactor dst) :
			color_op(op), color_src(src), color_dst(dst),
			alpha_op(op), alpha_src(src), alpha_dst(dst),
			mask(BlendMask::RGBA), rgba(0xffffffff) {}

		BlendMode(
			BlendOp color_op, BlendFactor color_src, BlendFactor color_dst,
			BlendOp alpha_op, BlendFactor alpha_src, BlendFactor alpha_dst,
			BlendMask blend_mask, u32 blend_rgba) :
			color_op(color_op), color_src(color_src), color_dst(color_dst),
			alpha_op(alpha_op), alpha_src(alpha_src), alpha_dst(alpha_dst),
			mask(blend_mask), rgba(blend_rgba) {}

		constexpr bool operator==(const BlendMode& rhs) const
		{
			return
				color_op == rhs.color_op && color_src == rhs.color_src && color_dst == rhs.color_dst &&
				alpha_op == rhs.alpha_op && alpha_src == rhs.alpha_src && alpha_dst == rhs.alpha_dst &&
				mask == rhs.mask && rgba == rhs.rgba;
		}

		constexpr bool operator!=(const BlendMode& rhs) const
		{
			return !(*this == rhs);
		}
	};

	// Texture filter
	enum class TextureFilter
	{
		None,   // Will fallback to whatever default the driver sets
		Linear, // Linear interpolation
		Nearest // Nearest Neighbour interpolation
	};

	// Texture Wrap Mode
	enum class TextureWrap
	{
		None,  // Will fallback to whatever default the driver sets
		Clamp, // Clamps the texture to the edges
		Repeat // Repeats the texture
	};

	// Texture Sampler State, applied during rendering
	struct TextureSampler
	{
		// Filter Mode
		TextureFilter filter;

		// Wrap X Mode
		TextureWrap wrap_x;

		// Wrap Y Mode
		TextureWrap wrap_y;

		TextureSampler() :
			filter(TextureFilter::Linear), wrap_x(TextureWrap::Repeat), wrap_y(TextureWrap::Repeat) {}

		TextureSampler(TextureFilter filter) :
			filter(filter), wrap_x(TextureWrap::Repeat), wrap_y(TextureWrap::Repeat) {}

		TextureSampler(TextureFilter filter, TextureWrap wrap_x, TextureWrap wrap_y) :
			filter(filter), wrap_x(wrap_x), wrap_y(wrap_y) {}

		bool operator==(const TextureSampler& rhs) const
		{
			return filter == rhs.filter && wrap_x == rhs.wrap_x && wrap_y == rhs.wrap_y;
		}

		bool operator!=(const TextureSampler& rhs) const
		{
			return !(*this == rhs);
		}
	};

	enum class TextureFormat
	{
		None,         // Invalid Format
		R,            // Single 8-bit channel
		RG,           // 2 8-bit channels
		RGBA,         // 4 8-bit channels
		DepthStencil, // Depth 24, Stencil 8
		Count         // Total Formats
	};

	enum class ClearMask
	{
		None = 0,
		Color = 1,
		Depth = 2,
		Stencil = 4,
		All = (int)Color | (int)Depth | (int)Stencil
	};

	// Supported Uniform Types
	enum class UniformType
	{
		None,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3x2,
		Mat4x4,
		Texture2D,
		Sampler2D
	};

	// Supported Shader Types
	enum class ShaderType
	{
		None = 0,
		Vertex = 1 << 0,
		Fragment = 1 << 1
	};

	// Uniform Info, provided by the Shader
	struct UniformInfo
	{
		// Name of the Uniform
		String name;

		// The Value type of the Uniform
		UniformType type;

		// The Shader type the Uniform is a part of
		ShaderType shader;

		// Texture / Sampler register index, which shaders can manually assign
		int register_index = 0;

		// Some rendering APIs have uniform buffers. The `buffer_index`
		// specifies which buffer the uniform belongs to
		int buffer_index = 0;

		// Array length of the Uniform (ex. a vec2[4] would be 4)
		int array_length = 0;
	};

	// Supported Vertex value types
	enum class VertexType
	{
		None,
		Float,
		Float2,
		Float3,
		Float4,
		Byte4,
		UByte4,
		Short2,
		UShort2,
		Short4,
		UShort4
	};

	// Vertex Attribute information
	struct VertexAttribute
	{
		// Location / Attribute Index
		int index = 0;

		// Vertex Type
		VertexType type = VertexType::None;

		// Whether the Vertex should be normalized (doesn't apply to Floats)
		bool normalized = false;
	};

	// Vertex Format information.
	// Holds a list of attributes and total stride per-vertex.
	struct VertexFormat
	{
		// List of Attributes
		StackVector<VertexAttribute, 16> attributes;

		// Total size in bytes of each Vertex element
		int stride = 0;

		VertexFormat() = default;
		VertexFormat(const StackVector<VertexAttribute, 16>& attributes, int stride = 0);
	};

	// Supported Vertex Index formats
	enum class IndexFormat
	{
		// Indices are 16-bit unsigned integers
		UInt16,

		// Indices are 32-bit unsigned integers
		UInt32
	};

	// Data to be passed to the shader to construct it
	struct ShaderData
	{
		struct HLSL_Attribute
		{
			// Semantic Name
			const char* semantic_name = nullptr;

			// (optional) Semantic Index
			int semantic_index = 0;
		};

		// Vertex Shader Program data
		String vertex;

		// Fragment Shader Program data
		String fragment;

		// HLSL Attributes - required for D3D11
		StackVector<HLSL_Attribute, 16> hlsl_attributes;
	};

	// A shader used during Rendering
	class Shader
	{
	protected:
		Shader() = default;

	public:
		// Copy / Moves not allowed
		Shader(const Shader&) = delete;
		Shader(Shader&&) = delete;
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&&) = delete;

		// Default Destructor
		virtual ~Shader() = default;

		// Creates a Shader with the given Shader Data.
		// If the Shader creation fails, it will return an invalid ShaderRef.
		static ShaderRef create(const ShaderData& data);

		// Gets a list of Shader Uniforms from Shader
		virtual Vector<UniformInfo>& uniforms() = 0;

		// Gets a list of Shader Uniforms from Shader
		virtual const Vector<UniformInfo>& uniforms() const = 0;
	};

	// A 2D Texture held by the GPU to be used during rendering
	class Texture
	{
	protected:
		Texture() = default;

	public:
		// Copy / Moves not allowed
		Texture(const Texture&) = delete;
		Texture(Texture&&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture& operator=(Texture&&) = delete;

		// Default Destructor
		virtual ~Texture() = default;

		// Creates a new Texture.
		// If the Texture creation fails, it will return an invalid TextureRef.
		static TextureRef create(const Image& image);

		// Creates a new Texture.
		// If image data is provided, it should be the full size of the texture.
		// If the Texture creation fails, it will return an invalid TextureRef.
		static TextureRef create(int width, int height, TextureFormat format, unsigned char* data = nullptr);

		// Creates a new Texture from a Stream.
		// If the Texture creation fails, it will return an invalid TextureRef.
		static TextureRef create(Stream& stream);

		// Creates a new Texture from a File.
		// If the Texture creation fails, it will return an invalid TextureRef.
		static TextureRef create(const FilePath& file);

		// gets the width of the texture
		virtual int width() const = 0;

		// gets the height of the texture
		virtual int height() const = 0;

		// Gets the format of the Texture
		virtual TextureFormat format() const = 0;

		// Sets the data of the Texture.
		// Note that the data should be the same format and size as the Texture. There is no row padding.
		virtual void set_data(const u8* data) = 0;

		// Sets the data of the Texture to the provided Color buffer.
		// If the Texture Format is not RGBA, this won't do anything.
		void set_data(const Color* data);

		// Gets the data of the Texture.
		// Note that the data will be written to in the same format as the Texture,
		// and you should allocate enough space for the full texture. There is no row padding.
		virtual void get_data(u8* data) = 0;

		// Gets the data of the Texture.
		// If the Texture Format is not RGBA, this won't do anything.
		void get_data(Color* data);

		// Returns true if the Texture is part of a FrameBuffer
		virtual bool is_framebuffer() const = 0;
	};

	// Up to 4 color textures + 1 depth/stencil
	using Attachments = StackVector<TextureRef, 5>;
	using AttachmentFormats = StackVector<TextureFormat, 5>;

	// Target is a 2D Buffer that can be drawn to.
	// It can hold up to 4 color Textures, and 1 Depth/Stencil Texture.
	class Target
	{
	protected:
		Target() = default;

	public:
		// Copy / Moves not allowed
		Target(const Target&) = delete;
		Target(Target&&) = delete;
		Target& operator=(const Target&) = delete;
		Target& operator=(Target&&) = delete;

		// Default Destructor
		virtual ~Target() = default;

		// Creates a new Target with a single Color texture
		// If the Target creation fails, it will return an invalid TargetRef.
		static TargetRef create(int width, int height);

		// Creates a new Target with the given Texture Attachments. You must provide at least one Attachment.
		// If the Target creation fails, it will return an invalid TargetRef.
		static TargetRef create(int width, int height, const AttachmentFormats& textures);

		// Gets the list of Attachments from the Target
		virtual Attachments& textures() = 0;

		// Gets the list of Attachments from the Target
		virtual const Attachments& textures() const = 0;

		// Gets the Attachment at a given index from the Target
		TextureRef& texture(int index);

		// Gets the Attachment at a given index from the Target
		const TextureRef& texture(int index) const;

		// Gets the width of the Target
		virtual int width() const;

		// Gets the height of the Target
		virtual int height() const;

		// Clears the Target
		virtual void clear(Color color = Color::black, float depth = 1.0f, u8 stencil = 0, ClearMask mask = ClearMask::All) = 0;
	};

	// A Mesh is a set of Indices and Vertices which are used for drawing
	class Mesh
	{
	protected:
		Mesh() = default;

	public:
		// Copy / Moves not allowed
		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) = delete;

		// Default Destructor
		virtual ~Mesh() = default;

		// Creates a new Mesh.
		// If the Mesh creation fails, it will return an invalid Mesh.
		static MeshRef create();

		// Uploads the given index buffer to the Mesh
		virtual void index_data(IndexFormat format, const void* indices, i64 count) = 0;

		// Uploads the given vertex buffer to the Mesh
		virtual void vertex_data(const VertexFormat& format, const void* vertices, i64 count) = 0;

		// Uploads the given instance buffer to the Mesh
		virtual void instance_data(const VertexFormat& format, const void* instances, i64 count) = 0;

		// Gets the index count of the Mesh
		virtual i64 index_count() const = 0;

		// Gets the vertex count of the Mesh
		virtual i64 vertex_count() const = 0;

		// Gets the instance count of the Mesh
		virtual i64 instance_count() const = 0;
	};

	// Materials hold values that can be assigned to a shader during rendering
	class Material final
	{
	private:
		Material(const ShaderRef& shader);

	public:
		// Copy / Moves not allowed
		Material(const Material&) = delete;
		Material(Material&&) = delete;
		Material& operator=(const Material&) = delete;
		Material& operator=(Material&&) = delete;

		// Default destructor
		~Material() = default;

		// Creates a new Material from the given Shader.
		// If the Shader is invalid, it will return an invalid MaterialRef.
		static MaterialRef create(const ShaderRef& shader);

		// Clones the material and returns a new one
		MaterialRef clone() const;

		// Returns the Shader assigned to the Material.
		ShaderRef shader() const;

		// Sets the texture
		void set_texture(const char* name, const TextureRef& texture, int array_index = 0);

		// Sets the texture
		void set_texture(int register_index, const TextureRef& texture);

		// Gets the texture, or an empty reference if invalid
		TextureRef get_texture(const char* name, int array_index = 0) const;

		// Gets the texture, or an empty reference if invalid
		TextureRef get_texture(int register_index) const;

		// Sets the sampler
		void set_sampler(const char* name, const TextureSampler& sampler, int array_index = 0);

		// Sets the sampler
		void set_sampler(int register_index, const TextureSampler& sampler);

		// Gets the sampler
		TextureSampler get_sampler(const char* name, int array_index = 0) const;

		// Gets the sampler
		TextureSampler get_sampler(int register_index) const;

		// Sets the value. `length` is the total number of floats to set
		// For example if the uniform is a float2[4], a total of 8 float values
		// can be set.
		void set_value(const char* name, const float* value, i64 length);

		// Shorthands to more easily assign uniform values
		void set_value(const char* name, float value);
		void set_value(const char* name, const Vec2f& value);
		void set_value(const char* name, const Vec3f& value);
		void set_value(const char* name, const Vec4f& value);
		void set_value(const char* name, const Mat3x2f& value);
		void set_value(const char* name, const Mat4x4f& value);
		void set_value(const char* name, const Vector<float>& value);
		void set_value(const char* name, const Vector<Vec2f>& value);
		void set_value(const char* name, const Vector<Vec3f>& value);
		void set_value(const char* name, const Vector<Vec4f>& value);
		void set_value(const char* name, const Vector<Mat3x2f>& value);
		void set_value(const char* name, const Vector<Mat4x4f>& value);

		// Gets a pointer to the values of the given Uniform, or nullptr if it doesn't exist.
		const float* get_value(const char* name, i64* length = nullptr) const;

		// Checks if the shader attached to the material has a uniform value with the given name
		bool has_value(const char* name) const;

		// Returns the internal Texture buffer
		const Vector<TextureRef>& textures() const;

		// Returns the internal Sampler buffer
		const Vector<TextureSampler>& samplers() const;

		// Returns the interal float buffer of all the values
		const float* data() const;

	private:
		ShaderRef m_shader;
		Vector<TextureRef> m_textures;
		Vector<TextureSampler> m_samplers;
		Vector<float> m_data;
	};

	// A single draw call
	struct DrawCall
	{
		// Framebuffer to draw to
		TargetRef target;

		// Mesh to draw with
		MeshRef mesh;

		// Material to draw with
		MaterialRef material;

		// Whether the DrawCall should use a specific viewport
		bool has_viewport;

		// Whether the DrawCall should use a scissor rectangle
		bool has_scissor;

		// The viewport (only used if hasViewport is true)
		Rectf viewport;

		// The scissor rectangle (only used if hasScissor is true)
		Rectf scissor;

		// First index in the Mesh to draw from
		i64 index_start;

		// Total amount of indices to draw from the Mesh
		i64 index_count;

		// Total amount of instances to draw from the Mesh
		i64 instance_count;

		// Depth Compare Function
		Compare depth;

		// Cull Mode
		Cull cull;

		// Blend Mode
		BlendMode blend;

		// Initializes a default DrawCall
		DrawCall();

		// Performs the render
		void perform();
	};
}