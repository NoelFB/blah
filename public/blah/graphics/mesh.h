#pragma once
#include <inttypes.h>
#include <blah/graphics/graphics.h>
#include <memory>

namespace Blah
{
	class Mesh
	{
	public:
		virtual ~Mesh() = default;

		// Sets the Vertex Format of the Mesh
		virtual void vertex_format(const VertexAttribute* attributes, int attribute_count, int stride = -1) = 0;

		// Sets the Instance Format of the Mesh
		virtual void instance_format(const VertexAttribute* attributes, int attribute_count, int stride = -1) = 0;

		// Uploads the given index buffer to the Mesh
		virtual void index_data(const void* indices, int64_t count) = 0;

		// Uploads the given vertex buffer to the Mesh
		// Note you must call vertex_format at least once before uploading vertices.
		virtual void vertex_data(const void* vertices, int64_t count) = 0;

		// Uploads the given instance buffer to the Mesh
		// Note you must call instance_format at least once before uploading instances.
		virtual void instance_data(const void* instances, int64_t count) = 0;

		// Gets the index count of the Mesh
		virtual int64_t index_count() const = 0;

		// Gets the vertex count of the Mesh
		virtual int64_t vertex_count() const = 0;

		// Gets the instance count of the Mesh
		virtual int64_t instance_count() const = 0;

		// Returns true if the Mesh is valid
		virtual bool is_valid() const = 0;

		// Destroys the given Mesh
		virtual void dispose() = 0;
	};

	typedef std::shared_ptr<Mesh> MeshRef;
}