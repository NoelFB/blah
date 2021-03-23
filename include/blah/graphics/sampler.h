#pragma once

namespace Blah
{
	// Texture filter
	enum class TextureFilter
	{
		// None will fallback to whatever default the driver sets
		None,

		// Linear interpolation
		Linear,

		// Nearest Neighbour interpolation
		Nearest
	};

	// Texture Wrap Mode
	enum class TextureWrap
	{
		// None will fallback to whatever default the driver sets
		None,

		// Clamps the texture to the edges
		Clamp,

		// Repeats the texture
		Repeat
	};

	// Texture Sampler State, applied during rendering
	struct TextureSampler
	{
		// Filter Mode
		TextureFilter filter;

		// Wrap X Mode
		TextureWrap wrap_x;

		// Wrap Y Mode
		TextureWrap wrap_y;

		TextureSampler() :
			filter(TextureFilter::Linear),
			wrap_x(TextureWrap::Repeat),
			wrap_y(TextureWrap::Repeat) {}

		TextureSampler(TextureFilter filter) :
			filter(filter),
			wrap_x(TextureWrap::Repeat),
			wrap_y(TextureWrap::Repeat) {}

		TextureSampler(TextureFilter filter, TextureWrap wrap_x, TextureWrap wrap_y) :
			filter(filter),
			wrap_x(wrap_x),
			wrap_y(wrap_y) {}

		bool operator==(const TextureSampler& rhs) const
		{
			return
				filter == rhs.filter &&
				wrap_x == rhs.wrap_x &&
				wrap_y == rhs.wrap_y;
		}

		bool operator!=(const TextureSampler& rhs) const
		{
			return !(*this == rhs);
		}
	};
}