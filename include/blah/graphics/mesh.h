#pragma once
#include <blah/core/common.h>
#include <memory>
#include <blah/containers/stackvector.h>

namespace Blah
{
	enum class VertexType
	{
		None,
		Float,
		Float2,
		Float3,
		Float4,
		Byte4,
		UByte4,
		Short2,
		UShort2,
		Short4,
		UShort4
	};

	struct VertexAttribute
	{
		// Location / Attribute Index
		int index = 0;

		// Vertex Type
		VertexType type = VertexType::None;

		// Whether the Vertex should be normalized (doesn't apply to Floats)
		bool normalized = false;
	};

	struct VertexFormat
	{
		// List of Attributes
		StackVector<VertexAttribute, 16> attributes;

		// Total size in bytes of each Vertex element
		int stride = 0;

		VertexFormat() = default;
		VertexFormat(std::initializer_list<VertexAttribute> attributes, int stride = 0);
	};

	enum class IndexFormat
	{
		UInt16,
		UInt32
	};

	class Mesh;
	typedef std::shared_ptr<Mesh> MeshRef;

	// A Mesh is a set of Indices and Vertices which are used for drawing
	class Mesh
	{
	protected:
		Mesh() = default;

	public:
		// Copy / Moves not allowed
		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) = delete;

		// Default Destructor
		virtual ~Mesh() = default;

		// Creates a new Mesh.
		// If the Mesh creation fails, it will return an invalid Mesh.
		static MeshRef create();

		// Uploads the given index buffer to the Mesh
		virtual void index_data(IndexFormat format, const void* indices, i64 count) = 0;

		// Uploads the given vertex buffer to the Mesh
		virtual void vertex_data(const VertexFormat& format, const void* vertices, i64 count) = 0;

		// Uploads the given instance buffer to the Mesh
		virtual void instance_data(const VertexFormat& format, const void* instances, i64 count) = 0;

		// Gets the index count of the Mesh
		virtual i64 index_count() const = 0;

		// Gets the vertex count of the Mesh
		virtual i64 vertex_count() const = 0;

		// Gets the instance count of the Mesh
		virtual i64 instance_count() const = 0;
	};
}