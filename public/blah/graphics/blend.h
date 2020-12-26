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
		BlendOp colorOp;
		BlendFactor colorSrc;
		BlendFactor colorDst;
		BlendOp alphaOp;
		BlendFactor alphaSrc;
		BlendFactor alphaDst;
		BlendMask mask;
		uint32_t rgba;

		BlendMode() = default;

		BlendMode(BlendOp op, BlendFactor src, BlendFactor dst)
		{
			colorOp = op;
			colorSrc = src;
			colorDst = dst;
			alphaOp = op;
			alphaSrc = src;
			alphaDst = dst;
			mask = BlendMask::RGBA;
			rgba = 0xffffffff;
		}

		BlendMode(BlendOp rgbOp, BlendFactor rgbSrc, BlendFactor rgbDst, BlendOp aOp, BlendFactor aSrc, BlendFactor aDst, BlendMask blendMask, uint32_t blendColor)
		{
			colorOp = rgbOp;
			colorSrc = rgbSrc;
			colorDst = rgbDst;
			alphaOp = aOp;
			alphaSrc = aSrc;
			alphaDst = aDst;
			mask = blendMask;
			rgba = blendColor;
		}

		inline bool operator==(const BlendMode& rhs) const {
			return colorOp == rhs.colorOp && colorSrc == rhs.colorSrc && colorDst == rhs.colorDst &&
				alphaOp == rhs.alphaOp && alphaSrc == rhs.alphaSrc && alphaDst == rhs.alphaDst &&
				mask == rhs.mask && rgba == rhs.rgba;
		}
		inline bool operator!=(const BlendMode& rhs) const { return !(*this == rhs); }

		static const BlendMode Normal;
		static const BlendMode Subtract;
	};
}