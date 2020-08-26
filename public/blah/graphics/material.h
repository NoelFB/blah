#pragma once
#include <blah/graphics/texture.h>
#include <blah/graphics/shader.h>
#include <blah/containers/list.h>
#include <memory>

namespace Blah
{
	class Material
	{
	public:
		Material(const ShaderRef& shader);
		~Material();

		Material(const Material& src) = delete;
		Material(Material&& src) = delete;
		Material& operator=(const Material& src) = delete;
		Material& operator=(Material&& src) = delete;

		// Returns the Shader assigned to the Material.
		const ShaderRef shader() const;

		// Sets the texture. If the index is out of bounds, the Material is not valid, the Uniform
		// is not a Texture Uniform, or the Uniform does not exist, this will do nothing.
		void set_texture(const char* name, const TextureRef& texture, int index = 0);

		// Gets the texture.
		// If the Uniform does not exist, or the index is out of bounds, this will return
		// an invalid Texture reference.
		TextureRef get_texture(const char* name, int index = 0) const;

		// Gets the texture from the given slot.
		// If the slot is not a Texture Uniform, or the index is out of bounds, this will return
		// an invalid Texture reference.
		TextureRef get_texture(int slot, int index = 0) const;

		// Sets the value. Length is the total amount of values to set. For example, if the Uniform type
		// is a float2, and there are 4 elements, the maximum length should be 8.
		void set_value(const char* name, const float* value, int64_t length);

		// Gets a pointer to the values of the given Uniform, or nullptr if it doesn't exist.
		// Length is the total amount of values. For example, if the Uniform type
		// is a float2, and there are 4 elements, the length should be 8.
		const float* get_value(const char* name, int64_t* length = nullptr) const;

		// Gets a pointer to the values of the given Uniform, or nullptr if it doesn't exist.
		// Length is the total amount of values. For example, if the Uniform type
		// is a float2, and there are 4 elements, the length should be 8.
		const float* get_value(int slot, int64_t* length = nullptr) const;

		// Returns true if the Material is valid
		bool is_valid() const;

		// Destroys the Material
		void dispose();

	private:
		ShaderRef m_shader;
		List<TextureRef> m_textures;
		List<float*> m_floats;
		float* m_data;
		bool m_disposed;
	};

	typedef std::shared_ptr<Material> MaterialRef;
}