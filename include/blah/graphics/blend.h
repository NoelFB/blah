#pragma once
#include <blah/core/common.h>

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

	// BlendMode using for rendering
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
		u32 rgba;

		BlendMode() = default;

		BlendMode(BlendOp op, BlendFactor src, BlendFactor dst) :
			color_op(op),
			color_src(src),
			color_dst(dst),
			alpha_op(op),
			alpha_src(src),
			alpha_dst(dst),
			mask(BlendMask::RGBA),
			rgba(0xffffffff) {}

		BlendMode(
			BlendOp color_op, BlendFactor color_src, BlendFactor color_dst,
			BlendOp alpha_op, BlendFactor alpha_src, BlendFactor alpha_dst,
			BlendMask blend_mask, u32 blend_rgba) :
			color_op(color_op),
			color_src(color_src),
			color_dst(color_dst),
			alpha_op(alpha_op),
			alpha_src(alpha_src),
			alpha_dst(alpha_dst),
			mask(blend_mask),
			rgba(blend_rgba) {}

		bool operator==(const BlendMode& rhs) const
		{
			return
				color_op == rhs.color_op && color_src == rhs.color_src && color_dst == rhs.color_dst &&
				alpha_op == rhs.alpha_op && alpha_src == rhs.alpha_src && alpha_dst == rhs.alpha_dst &&
				mask == rhs.mask && rgba == rhs.rgba;
		}

		bool operator!=(const BlendMode& rhs) const
		{
			return !(*this == rhs);
		}
	};
}