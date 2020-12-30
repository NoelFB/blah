#pragma once

namespace Blah
{
	enum class TextureFilter
	{
		None,
		Linear,
		Nearest
	};

	enum class TextureWrap
	{
		None,
		Clamp,
		Repeat
	};

	struct TextureSampler
	{
		TextureFilter filter;
		TextureWrap wrap_x;
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