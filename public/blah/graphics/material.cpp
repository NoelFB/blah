#include <blah/graphics/material.h>
#include <blah/log.h>

using namespace Blah;

namespace
{
	int calc_uniform_size(const ShaderUniform& uniform)
	{
		int components = 0;

		switch (uniform.type)
		{
		case UniformType::Float: components = 1; break;
		case UniformType::Float2: components = 2; break;
		case UniformType::Float3: components = 3; break;
		case UniformType::Float4: components = 4; break;
		case UniformType::Mat3x2: components = 6; break;
		case UniformType::Mat4x4: components = 16; break;
		default:
			BLAH_ERROR("Unespected Uniform Type");
			break;
		}

		return components * uniform.array_length;
	}
}

Material::Material(const ShaderRef& shader)
{
	m_shader = shader;
	m_data = nullptr;
	m_disposed = false;

	// invalid shader
	if (!m_shader || !m_shader->is_valid())
	{
		m_disposed = true;
		return;
	}

	Uniforms uniforms = shader->uniforms();
	StackVector<size_t, BLAH_UNIFORMS> float_offsets;
	size_t float_size = 0;

	for (auto& uniform : uniforms)
	{
		if (uniform.type == UniformType::None)
			continue;

		if (uniform.type == UniformType::Texture)
		{
			for (int i = 0; i < uniform.array_length; i ++)
				m_textures.push_back(TextureRef());
			continue;
		}

		float_offsets.push_back(float_size);
		float_size += calc_uniform_size(uniform);
	}

	m_data = new float[float_size];
	memset(m_data, 0, sizeof(float) * float_size);

	for (auto& it : float_offsets)
		m_floats.push_back(m_data + it);
}

Material::~Material()
{
	dispose();
}

const ShaderRef Material::shader() const
{
	return m_shader;
}

void Material::set_texture(const char* name, const TextureRef& texture, int index)
{
	BLAH_ASSERT(!m_disposed, "Material has been disposed");
	BLAH_ASSERT(m_shader && m_shader->is_valid(), "Material Shader is invalid");

	if (m_textures.size() > 0)
	{
		int offset = 0;
		for (auto& uniform : m_shader->uniforms())
		{
			if (uniform.type != UniformType::Texture)
				continue;

			if (strcmp(uniform.name, name) == 0)
			{
				m_textures[offset + index] = texture;
				return;
			}

			offset += uniform.array_length;
			if (offset + index >= m_textures.size())
				break;
		}
	}

	Log::warn("No Texture Uniform '%s' at index [%i] exists", name, index);
}

TextureRef Material::get_texture(const char* name, int index) const
{
	BLAH_ASSERT(!m_disposed, "Material has been disposed");
	BLAH_ASSERT(m_shader && m_shader->is_valid(), "Material Shader is invalid");

	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type != UniformType::Texture)
			continue;

		if (strcmp(uniform.name, name) == 0)
			return m_textures[offset + index];

		offset += uniform.array_length;
		if (offset + index >= m_textures.size())
			break;
	}

	Log::warn("No Texture Uniform '%s' at index [%i] exists", name, index);
	return TextureRef();
}

TextureRef Material::get_texture(int slot, int index) const
{
	BLAH_ASSERT(!m_disposed, "Material has been disposed");
	BLAH_ASSERT(m_shader && m_shader->is_valid(), "Material Shader is invalid");

	int offset = 0;
	int s = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type == UniformType::Texture)
		{
			if (s == slot)
				return m_textures[offset + index];

			offset += uniform.array_length;
			if (offset + index >= m_textures.size())
				break;
		}

		s++;
	}

	Log::warn("No Texture Uniform ['%i'] at index [%i] exists", slot, index);
	return TextureRef();
}

void Material::set_value(const char* name, const float* value, int64_t length)
{
	BLAH_ASSERT(!m_disposed, "Material has been disposed");
	BLAH_ASSERT(m_shader && m_shader->is_valid(), "Material Shader is invalid");
	BLAH_ASSERT(length >= 0, "Length must be >= 0");

	int index = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type == UniformType::Texture || uniform.type == UniformType::None)
			continue;

		if (strcmp(uniform.name, name) == 0)
		{
			auto max = calc_uniform_size(uniform);
			if (length > max)
			{
				Log::warn("Exceeding length of Uniform '%s' (%i / %i)", name, length, max);
				length = max;
			}

			memcpy(m_floats[index], value, sizeof(float) * length);
			return;
		}

		index++;
	}

	Log::warn("No Uniform '%s' exists", name);
}

const float* Material::get_value(const char* name, int64_t* length) const
{
	BLAH_ASSERT(!m_disposed, "Material has been disposed");
	BLAH_ASSERT(m_shader && m_shader->is_valid(), "Material Shader is invalid");

	int index = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type == UniformType::Texture || uniform.type == UniformType::None)
			continue;

		if (strcmp(uniform.name, name) == 0)
		{
			if (length != nullptr)
				*length = calc_uniform_size(uniform);
			return m_floats[index];
		}

		index++;
	}

	*length = 0;
	return nullptr;
	Log::warn("No Uniform '%s' exists", name);
}

const float* Material::get_value(int slot, int64_t* length) const
{
	BLAH_ASSERT(!m_disposed, "Material has been disposed");
	BLAH_ASSERT(m_shader && m_shader->is_valid(), "Material Shader is invalid");

	int index = 0;
	int s = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type != UniformType::Texture && uniform.type != UniformType::None)
		{
			if (index == slot)
			{
				if (length != nullptr)
					*length = calc_uniform_size(uniform);
				return m_floats[index];
			}
			index++;
		}

		s++;
	}

	*length = 0;
	return nullptr;
	Log::warn("No Uniform [%i] exists", slot);
}

bool Material::is_valid() const
{
	return !m_disposed && m_shader && m_shader->is_valid();
}

void Material::dispose()
{
	delete[] m_data;
	m_data = nullptr;
	m_shader.reset();
	m_textures.clear();
	m_floats.clear();
}