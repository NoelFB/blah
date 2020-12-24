#pragma once
#include <blah/graphics/graphics.h>
#include <blah/containers/stackvector.h>
#include <memory>

namespace Blah
{
	typedef StackVector<ShaderUniform, BLAH_UNIFORMS> Uniforms;
	typedef StackVector<ShaderAttribute, BLAH_ATTRIBUTES> Attributes;

	class Shader
	{
	public:
		virtual ~Shader() = default;

		// Gets a list of Shader Uniforms from Shader
		virtual Uniforms& uniforms() = 0;

		// Gets a list of Shader Uniforms from Shader
		virtual const Uniforms& uniforms() const = 0;

		// Gets a list of Shader Attributes from Shader
		virtual Attributes& attributes() = 0;

		// Gets a list of Shader Attributes from Shader
		virtual const Attributes& attributes() const = 0;
	};

	typedef std::shared_ptr<Shader> ShaderRef;
}