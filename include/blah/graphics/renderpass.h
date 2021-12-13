#pragma once
#include <blah/common.h>
#include <blah/numerics/spatial.h>
#include <blah/containers/str.h>
#include <blah/graphics/texture.h>
#include <blah/graphics/target.h>
#include <blah/graphics/mesh.h>
#include <blah/graphics/shader.h>
#include <blah/graphics/material.h>
#include <blah/graphics/blend.h>

namespace Blah
{
	// Depth comparison function to use during a draw call
	enum class Compare
	{
		None,
		Always,
		Never,
		Less,
		Equal,
		LessOrEqual,
		Greater,
		NotEqual,
		GreatorOrEqual
	};

	// Cull mode during a draw call
	enum class Cull
	{
		// No Culling enabled
		None = 0,

		// Cull front faces
		Front = 1,

		// Cull back faces
		Back = 2,
	};

	// A single draw call
	struct RenderPass
	{
		// Framebuffer to draw to
		TargetRef target;

		// Mesh to draw with
		MeshRef mesh;

		// Material to draw with
		MaterialRef material;

		// Whether the RenderPass should use a specific viewport
		bool has_viewport;

		// Whether the RenderPass should use a scissor rectangle
		bool has_scissor;

		// The viewport (only used if hasViewport is true)
		Rectf viewport;

		// The scissor rectangle (only used if hasScissor is true)
		Rectf scissor;

		// First index in the Mesh to draw from
		i64 index_start;

		// Total amount of indices to draw from the Mesh
		i64 index_count;

		// Total amount of instances to draw from the Mesh
		i64 instance_count;

		// Depth Compare Function
		Compare depth;

		// Cull Mode
		Cull cull;

		// Blend Mode
		BlendMode blend;

		// Initializes a default RenderPass
		RenderPass();

		// Performs the render
		void perform();
	};
}