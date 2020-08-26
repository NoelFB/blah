#pragma once
#include <blah/graphics/graphics.h>
#include <blah/containers/stacklist.h>
#include <memory>

namespace Blah
{
	typedef StackList<ShaderUniform, BLAH_UNIFORMS> Uniforms;
	typedef StackList<ShaderAttribute, BLAH_ATTRIBUTES> Attributes;

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

		// Returns true if the Shader is valid
		virtual bool is_valid() const = 0;

		// Destroys the given Shader
		virtual void dispose() = 0;
	};

	typedef std::shared_ptr<Shader> ShaderRef;
}