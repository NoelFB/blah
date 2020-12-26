#pragma once
#include <blah/containers/stackvector.h>
#include <blah/containers/str.h>
#include <memory>

namespace Blah
{
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

	struct UniformInfo
	{
		String name;
		UniformType type;
		int array_length;
	};

	struct ShaderData
	{
		const char* vertex;
		const char* fragment;
	};

	class Shader;
	typedef std::shared_ptr<Shader> ShaderRef;

	class Shader
	{
	public:

		// Creates a Shader with the given Shader Data.
		// If the Shader creation fails, it will return an invalid ShaderRef.
		static ShaderRef create(const ShaderData* data);

		virtual ~Shader() = default;

		// Gets a list of Shader Uniforms from Shader
		virtual Vector<UniformInfo>& uniforms() = 0;

		// Gets a list of Shader Uniforms from Shader
		virtual const Vector<UniformInfo>& uniforms() const = 0;
	};

}