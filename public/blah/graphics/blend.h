#pragma once
#include <inttypes.h>

namespace Blah
{
	enum class BlendOp
	{
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max
	};

	enum class BlendFactor
	{
		Zero,
		One,
		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha,
		ConstantColor,
		OneMinusConstantColor,
		ConstantAlpha,
		OneMinusConstantAlpha,
		SrcAlphaSaturate,
		Src1Color,
		OneMinusSrc1Color,
		Src1Alpha,
		OneMinusSrc1Alpha
	};

	enum class BlendMask
	{
		None = 0,
		Red = 1,
		Green = 2,
		Blue = 4,
		Alpha = 8,
		RGB = Red | Green | Blue,
		RGBA = Red | Green | Blue | Alpha,
	};

	struct BlendMode
	{
		// Normal, pre-multiplied, Blend Mode
		static const BlendMode Normal;

		// Subtractive Blend Mode
		static const BlendMode Subtract;

		BlendOp color_op;
		BlendFactor color_src;
		BlendFactor color_dst;
		BlendOp alpha_op;
		BlendFactor alpha_src;
		BlendFactor alpha_dst;
		BlendMask mask;
		uint32_t rgba;

		BlendMode() = default;

		BlendMode(BlendOp op, BlendFactor src, BlendFactor dst)
		{
			color_op = op;
			color_src = src;
			color_dst = dst;
			alpha_op = op;
			alpha_src = src;
			alpha_dst = dst;
			mask = BlendMask::RGBA;
			rgba = 0xffffffff;
		}

		BlendMode(BlendOp rgbOp, BlendFactor rgbSrc, BlendFactor rgbDst, BlendOp aOp, BlendFactor aSrc, BlendFactor aDst, BlendMask blendMask, uint32_t blendColor)
		{
			color_op = rgbOp;
			color_src = rgbSrc;
			color_dst = rgbDst;
			alpha_op = aOp;
			alpha_src = aSrc;
			alpha_dst = aDst;
			mask = blendMask;
			rgba = blendColor;
		}

		inline bool operator==(const BlendMode& rhs) const
		{
			return
				color_op == rhs.color_op && color_src == rhs.color_src && color_dst == rhs.color_dst &&
				alpha_op == rhs.alpha_op && alpha_src == rhs.alpha_src && alpha_dst == rhs.alpha_dst &&
				mask == rhs.mask && rgba == rhs.rgba;
		}

		inline bool operator!=(const BlendMode& rhs) const
		{
			return !(*this == rhs);
		}
	};
}