#include <blah/graphics/shader.h>
#include <blah/internal/graphics_backend.h>

using namespace Blah;

ShaderRef Shader::create(const ShaderData* data)
{
	return GraphicsBackend::create_shader(data);
}