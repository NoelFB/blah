#pragma once
#include <blah/common.h>
#include <blah/graphics/texture.h>
#include <blah/graphics/shader.h>
#include <blah/graphics/sampler.h>
#include <blah/containers/vector.h>

namespace Blah
{
	class Material;
	using MaterialRef = Ref<Material>;

	// Materials hold values that can be assigned to a shader during rendering
	class Material final
	{
	private:
		Material(const ShaderRef& shader);

	public:
		// Copy / Moves not allowed
		Material(const Material&) = delete;
		Material(Material&&) = delete;
		Material& operator=(const Material&) = delete;
		Material& operator=(Material&&) = delete;

		// Default destructor
		~Material() = default;

		// Creates a new Material from the given Shader.
		// If the Shader is invalid, it will return an invalid MaterialRef.
		static MaterialRef create(const ShaderRef& shader);

		// Returns the Shader assigned to the Material.
		ShaderRef shader() const;

		// Sets the texture
		void set_texture(const char* name, const TextureRef& texture, int array_index = 0);

		// Sets the texture
		void set_texture(int slot, const TextureRef& texture, int array_index = 0);

		// Gets the texture, or an empty reference if invalid
		TextureRef get_texture(const char* name, int array_index = 0) const;

		// Gets the texture, or an empty reference if invalid
		TextureRef get_texture(int slot, int array_index = 0) const;

		// Sets the sampler
		void set_sampler(const char* name, const TextureSampler& sampler, int array_index = 0);

		// Sets the sampler
		void set_sampler(int slot, const TextureSampler& sampler, int array_index = 0);

		// Gets the sampler
		TextureSampler get_sampler(const char* name, int array_index = 0) const;

		// Gets the sampler
		TextureSampler get_sampler(int slot, int array_index = 0) const;

		// Sets the value. `length` is the total number of floats to set
		// For example if the uniform is a float2[4], a total of 8 float values
		// can be set.
		void set_value(const char* name, const float* value, i64 length);

		// Gets a pointer to the values of the given Uniform, or nullptr if it doesn't exist.
		const float* get_value(const char* name, i64* length = nullptr) const;

		// Checks if the shader attached to the material has a uniform value with the given name
		bool has_value(const char* name) const;

		// Returns the internal Texture buffer
		const Vector<TextureRef>& textures() const;

		// Returns the internal Sampler buffer
		const Vector<TextureSampler>& samplers() const;

		// Returns the interal float buffer of all the values
		const float* data() const;

	private:
		ShaderRef m_shader;
		Vector<TextureRef> m_textures;
		Vector<TextureSampler> m_samplers;
		Vector<float> m_data;
	};
}