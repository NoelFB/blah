#pragma once
#include <inttypes.h>
#include <memory>

namespace Blah
{
	enum class VertexSemantics
	{
		None,
		Position,
		Normal,
		Bitangent,
		Color0,
		Color1,
		Color2,
		Color3,
		Indices,
		Weight,
		Texcoord0,
		Texcoord1,
		Texcoord2,
		Texcoord3,
		Texcoord4,
		Texcoord5,
		Texcoord6,
		Texcoord7
	};

	enum class VertexAttributeType
	{
		None,
		Byte,
		Short,
		Int,
		Float
	};

	struct VertexAttribute
	{
		int index;
		VertexSemantics semantics;
		VertexAttributeType type;
		int components;
		bool normalized;
	};

	class Mesh;
	typedef std::shared_ptr<Mesh> MeshRef;

	class Mesh
	{
	public:
		// Creates a new Mesh.
		// If the Mesh creation fails, it will return an invalid Mesh.
		static MeshRef create();

		virtual ~Mesh() = default;

		// Sets the Vertex Format of the Mesh
		void vertex_format(const VertexAttribute* attributes, int attribute_count, int stride = -1);

		// Sets the Instance Format of the Mesh
		void instance_format(const VertexAttribute* attributes, int attribute_count, int stride = -1);

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

	protected:
		virtual void vertex_format_internal(const VertexAttribute* attributes, int count, int stride) = 0;
		virtual void instance_format_internal(const VertexAttribute* attributes, int count, int stride) = 0;
	};
}