#include "mesh.h"
#include <blah/internal/graphics_backend.h>

using namespace Blah;

MeshRef Mesh::create()
{
	return GraphicsBackend::create_mesh();
}

void Mesh::vertex_format(const VertexAttribute* attributes, int attribute_count, int stride)
{
	if (stride < 0)
	{
		stride = 0;

		for (int n = 0; n < attribute_count; n++)
		{
			const VertexAttribute* attrib = (attributes + n);

			if (attrib->type == VertexAttributeType::Byte)
				stride += attrib->components * 1;
			else if (attrib->type == VertexAttributeType::Short)
				stride += attrib->components * 2;
			else if (attrib->type == VertexAttributeType::Int)
				stride += attrib->components * 4;
			else if (attrib->type == VertexAttributeType::Float)
				stride += attrib->components * 4;
		}
	}

	vertex_format_internal(attributes, attribute_count, stride);
}

void Mesh::instance_format(const VertexAttribute* attributes, int attribute_count, int stride)
{
	if (stride < 0)
	{
		stride = 0;

		for (int n = 0; n < attribute_count; n++)
		{
			const VertexAttribute* attrib = (attributes + n);

			if (attrib->type == VertexAttributeType::Byte)
				stride += attrib->components * 1;
			else if (attrib->type == VertexAttributeType::Short)
				stride += attrib->components * 2;
			else if (attrib->type == VertexAttributeType::Int)
				stride += attrib->components * 4;
			else if (attrib->type == VertexAttributeType::Float)
				stride += attrib->components * 4;
		}
	}

	instance_format_internal(attributes, attribute_count, stride);
}
