#include <blah/graphics/material.h>
#include <blah/common.h>
#include <cstring>

using namespace Blah;

namespace
{
	int blah_calc_uniform_size(const UniformInfo& uniform)
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
			BLAH_ASSERT(false, "Unespected Uniform Type");
			break;
		}

		return components * uniform.array_length;
	}
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

		if (uniform.type == UniformType::Texture2D)
		{
			for (int i = 0; i < uniform.array_length; i ++)
				m_textures.push_back(TextureRef());
			continue;
		}

		if (uniform.type == UniformType::Sampler2D)
		{
			for (int i = 0; i < uniform.array_length; i++)
				m_samplers.push_back(TextureSampler());
			continue;
		}

		float_size += blah_calc_uniform_size(uniform);
	}

	m_data.expand(float_size);
}

MaterialRef Material::create(const ShaderRef& shader)
{
	BLAH_ASSERT(shader, "The provided shader is invalid");

	if (shader)
		return MaterialRef(new Material(shader));

	return MaterialRef();
}

MaterialRef Material::clone() const
{
	auto copy = MaterialRef(new Material(m_shader));
	copy->m_textures = m_textures;
	copy->m_samplers = m_samplers;
	copy->m_data = m_data;
	return copy;
}

ShaderRef Material::shader() const
{
	return m_shader;
}

void Material::set_texture(const char* name, const TextureRef& texture, int index)
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type != UniformType::Texture2D)
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
		if (uniform.type != UniformType::Texture2D)
			continue;

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

TextureRef Material::get_texture(const char* name, int index) const
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type != UniformType::Texture2D)
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
		if (uniform.type != UniformType::Texture2D)
			continue;

		if (s == slot)
		{
			if (index > uniform.array_length)
				break;

			return m_textures[offset + index];
		}

		offset += uniform.array_length;
		if (offset + index >= m_textures.size())
			break;

		s++;
	}

	Log::warn("No Texture Uniform ['%i'] at index [%i] exists", slot, index);
	return TextureRef();
}

void Material::set_sampler(const char* name, const TextureSampler& sampler, int index)
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type != UniformType::Sampler2D)
			continue;

		if (strcmp(uniform.name, name) == 0)
		{
			m_samplers[offset + index] = sampler;
			return;
		}

		offset += uniform.array_length;
		if (offset + index >= m_samplers.size())
			break;
	}

	Log::warn("No Sampler Uniform '%s' at index [%i] exists", name, index);
}

void Material::set_sampler(int slot, const TextureSampler& sampler, int index)
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	int s = 0;
	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type != UniformType::Sampler2D)
			continue;

		if (s == slot)
		{
			if (index > uniform.array_length)
				break;

			m_samplers[offset + index] = sampler;
			break;
		}

		offset += uniform.array_length;
		s++;
	}
}

TextureSampler Material::get_sampler(const char* name, int index) const
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type != UniformType::Sampler2D)
			continue;

		if (strcmp(uniform.name, name) == 0)
			return m_samplers[offset + index];

		offset += uniform.array_length;
		if (offset + index >= m_samplers.size())
			break;
	}

	Log::warn("No Sampler Uniform '%s' at index [%i] exists", name, index);
	return TextureSampler();
}

TextureSampler Material::get_sampler(int slot, int index) const
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	int s = 0;
	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type != UniformType::Sampler2D)
			continue;

		if (s == slot)
		{
			if (index > uniform.array_length)
				break;

			return m_samplers[offset + index];
		}

		offset += uniform.array_length;
		if (offset + index >= m_samplers.size())
			break;

		s++;
	}

	Log::warn("No Sampler Uniform ['%i'] at index [%i] exists", slot, index);
	return TextureSampler();
}

void Material::set_value(const char* name, const float* value, i64 length)
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");
	BLAH_ASSERT(length >= 0, "Length must be >= 0");

	int index = 0;
	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type == UniformType::Texture2D ||
			uniform.type == UniformType::Sampler2D ||
			uniform.type == UniformType::None)
			continue;

		if (strcmp(uniform.name, name) == 0)
		{
			auto max = blah_calc_uniform_size(uniform);
			if (length > max)
			{
				Log::warn("Exceeding length of Uniform '%s' (%i / %i)", name, length, max);
				length = max;
			}

			memcpy(m_data.begin() + offset, value, sizeof(float) * length);
			return;
		}

		offset += blah_calc_uniform_size(uniform);
		index++;
	}

	Log::warn("No Uniform '%s' exists", name);
}

void Material::set_value(const char* name, float value)
{
	set_value(name, &value, 1);
}

void Material::set_value(const char* name, const Vec2f& value)
{
	set_value(name, &value.x, 2);
}

void Material::set_value(const char* name, const Vec3f& value)
{
	set_value(name, &value.x, 3);
}

void Material::set_value(const char* name, const Vec4f& value)
{
	set_value(name, &value.x, 4);
}

void Material::set_value(const char* name, const Mat3x2f& value)
{
	set_value(name, &value.m11, 6);
}

void Material::set_value(const char* name, const Mat4x4f& value)
{
	set_value(name, &value.m11, 16);
}

void Material::set_value(const char* name, const Vector<float>& value)
{
	set_value(name, value.data(), value.size());
}

void Material::set_value(const char* name, const Vector<Vec2f>& value)
{
	set_value(name, (float*)value.data(), value.size() * 2);
}

void Material::set_value(const char* name, const Vector<Vec3f>& value)
{
	set_value(name, (float*)value.data(), value.size() * 3);
}

void Material::set_value(const char* name, const Vector<Vec4f>& value)
{
	set_value(name, (float*)value.data(), value.size() * 4);
}

void Material::set_value(const char* name, const Vector<Mat3x2f>& value)
{
	set_value(name, (float*)value.data(), value.size() * 6);
}

void Material::set_value(const char* name, const Vector<Mat4x4f>& value)
{
	set_value(name, (float*)value.data(), value.size() * 16);
}

const float* Material::get_value(const char* name, i64* length) const
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	int index = 0;
	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type == UniformType::Texture2D ||
			uniform.type == UniformType::Sampler2D ||
			uniform.type == UniformType::None)
			continue;

		if (strcmp(uniform.name, name) == 0)
		{
			if (length != nullptr)
				*length = blah_calc_uniform_size(uniform);
			return m_data.begin() + offset;
		}

		index++;
		offset += blah_calc_uniform_size(uniform);
	}

	Log::warn("No Uniform '%s' exists", name);
	*length = 0;
	return nullptr;
}

bool Material::has_value(const char* name) const
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");
	
	if (name != nullptr && name[0] != '\0')
	{
		for (auto& uniform : m_shader->uniforms())
			if (strcmp(uniform.name, name) == 0)
				return true;
	}

	return false;
}

const Vector<TextureRef>& Material::textures() const
{
	return m_textures;
}

const Vector<TextureSampler>& Material::samplers() const
{
	return m_samplers;
}

const float* Material::data() const
{
	return m_data.begin();
}
