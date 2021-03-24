#include <blah/graphics/blend.h>

using namespace Blah;

const BlendMode BlendMode::Normal = BlendMode(
	BlendOp::Add,
	BlendFactor::One,
	BlendFactor::OneMinusSrcAlpha,
	BlendOp::Add,
	BlendFactor::One,
	BlendFactor::OneMinusSrcAlpha,
	BlendMask::RGBA,
	0xffffffff
);

const BlendMode BlendMode::Subtract = BlendMode(
	BlendOp::ReverseSubtract,
	BlendFactor::One,
	BlendFactor::One,
	BlendOp::Add,
	BlendFactor::One,
	BlendFactor::One,
	BlendMask::RGBA,
	0xffffffff
);
