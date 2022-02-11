#include <blah/graphics/mesh.h>
#include "../internal/renderer.h"

using namespace Blah;

MeshRef Mesh::create()
{
	BLAH_ASSERT_RENDERER();

	if (Renderer::instance)
		return Renderer::instance->create_mesh();

	return MeshRef();
}

VertexFormat::VertexFormat(const StackVector<VertexAttribute, 16>& attr, int stride)
{
	attributes = attr;

	if (stride <= 0)
	{
		stride = 0;

		for (auto& it : attributes)
		{
			switch (it.type)
			{
			case VertexType::None: break;
			case VertexType::Float: stride += 4; break;
			case VertexType::Float2: stride += 8; break;
			case VertexType::Float3: stride += 12; break;
			case VertexType::Float4: stride += 16; break;
			case VertexType::Byte4: stride += 4; break;
			case VertexType::UByte4: stride += 4; break;
			case VertexType::Short2: stride += 4; break;
			case VertexType::UShort2: stride += 4; break;
			case VertexType::Short4: stride += 8; break;
			case VertexType::UShort4: stride += 8; break;
			}
		}
	}

	this->stride = stride;
}
