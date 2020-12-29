#include <blah/graphics/material.h>
#include <blah/log.h>

using namespace Blah;

namespace
{
	int calc_uniform_size(const UniformInfo& uniform)
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

MaterialRef Material::create(const ShaderRef& shader)
{
	BLAH_ASSERT(shader, "The provided shader is invalid");

	if (shader)
		return MaterialRef(new Material(shader));

	return MaterialRef();
}

Material::Material(const ShaderRef& shader)
{
	BLAH_ASSERT(shader, "Material is being created with an invalid shader");
	m_shader = shader;

	auto& uniforms = shader->uniforms();
	int float_size = 0;

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

		float_size += calc_uniform_size(uniform);
	}

	m_data.expand(float_size);
}

const ShaderRef Material::shader() const
{
	return m_shader;
}

void Material::set_texture(const char* name, const TextureRef& texture, int index)
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

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

	Log::warn("No Texture Uniform '%s' at index [%i] exists", name, index);
}

void Material::set_texture(int slot, const TextureRef& texture, int index)
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	int s = 0;
	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type == UniformType::Texture)
		{
			if (s == slot)
			{
				if (index > uniform.array_length)
					break;

				m_textures[offset + index] = texture;
				break;
			}
			offset += uniform.array_length;
			s++;
		}
	}
}

TextureRef Material::get_texture(const char* name, int index) const
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

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
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	int s = 0;
	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type == UniformType::Texture)
		{
			if (s == slot)
			{
				if (index > uniform.array_length)
					break;

				return m_textures[offset + index];
			}

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
	BLAH_ASSERT(m_shader, "Material Shader is invalid");
	BLAH_ASSERT(length >= 0, "Length must be >= 0");

	int index = 0;
	int offset = 0;
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

			memcpy(m_data.begin() + offset, value, sizeof(float) * length);
			return;
		}

		offset += calc_uniform_size(uniform);
		index++;
	}

	Log::warn("No Uniform '%s' exists", name);
}

const float* Material::get_value(const char* name, int64_t* length) const
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	int index = 0;
	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type == UniformType::Texture || uniform.type == UniformType::None)
			continue;

		if (strcmp(uniform.name, name) == 0)
		{
			if (length != nullptr)
				*length = calc_uniform_size(uniform);
			return m_data.begin() + offset;
		}

		index++;
		offset += calc_uniform_size(uniform);
	}

	*length = 0;
	return nullptr;
	Log::warn("No Uniform '%s' exists", name);
}

const Vector<TextureRef>& Material::textures() const
{
	return m_textures;
}

const float* Material::data() const
{
	return m_data.begin();
}
