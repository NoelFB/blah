#pragma once
#include <blah/containers/stackvector.h>
#include <blah/containers/str.h>
#include <memory>

namespace Blah
{
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
		Texture
	};

	// Supported Shader Types
	enum class ShaderType
	{
		None		= 0,
		Vertex		= 1 << 0,
		Fragment	= 1 << 1
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

		// Some rendering APIs have uniform buffers. The `buffer_index`
		// specifies which buffer the uniform belongs to
		int buffer_index;

		// Array length of the Uniform (ex. a vec2[4] would be 4)
		int array_length;
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

	class Shader;
	typedef std::shared_ptr<Shader> ShaderRef;

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
		static ShaderRef create(const ShaderData* data);

		// Gets a list of Shader Uniforms from Shader
		virtual Vector<UniformInfo>& uniforms() = 0;

		// Gets a list of Shader Uniforms from Shader
		virtual const Vector<UniformInfo>& uniforms() const = 0;
	};

}