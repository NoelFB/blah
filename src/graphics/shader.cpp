#include <blah/graphics/shader.h>
#include <blah/app.h>
#include "../internal/internal.h"

using namespace Blah;

ShaderRef Shader::create(const ShaderData& data)
{
	BLAH_ASSERT_RENDERER();
	BLAH_ASSERT(data.vertex.length() > 0, "Must provide a Vertex Shader");
	BLAH_ASSERT(data.fragment.length() > 0, "Must provide a Fragment Shader");
	BLAH_ASSERT(data.hlsl_attributes.size() > 0 || App::renderer().type != RendererType::D3D11, "D3D11 Shaders must have hlsl_attributes assigned");

	ShaderRef shader;

	if (App::Internal::renderer)
		shader = App::Internal::renderer->create_shader(&data);
	
	// validate the shader
	if (shader)
	{
		auto& uniforms = shader->uniforms();

		// make sure its uniforms are valid
		for (auto& it : uniforms)
			if (it.type == UniformType::None)
			{
				auto error = String::fmt("Uniform '%s' has an invalid type!\n\tOnly Float/Float2/Float3/Float4/Mat3x2/Mat4x4/Texture are allowed!", it.name.cstr());
				BLAH_ASSERT(false, error.cstr());
				return ShaderRef();
			}

		// make sure uniform names don't overlap
		for (int i = 0; i < uniforms.size(); i ++)
			for (int j = i + 1; j < uniforms.size(); j ++)
				if (uniforms[i].name == uniforms[j].name)
				{
					auto error = String::fmt("Shader Uniform names '%s' overlap! All Names must be unique.", uniforms[0].name.cstr());
					BLAH_ASSERT(false, error.cstr());
					return ShaderRef();
				}
	}

	return shader;
}
