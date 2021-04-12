#pragma once
#include <blah/images/image.h>
#include <blah/math/color.h>
#include <blah/math/rectI.h>
#include <blah/math/point.h>
#include <blah/containers/str.h>
#include <blah/containers/vector.h>
#include <blah/streams/bufferstream.h>
#include <blah/core/filesystem.h>

namespace Blah
{
	// Texture Packer, which takes source images and combines
	// them into a single large texture. Useful for 2D sprite batching.
	class Packer
	{
	public:

		// Packer Entry, which stores information about the resulting packed texture
		class Entry
		{
		friend class Packer;
		private:
			i64 memory_index;

		public:

			// entry ID
			u64 id;

			// Texture Page that it was packed into.
			// This won't be set until after the packer has run.
			int page;

			// Whether the entry is empty
			bool empty;

			// Packed frame rectangle.
			// This won't be set until after the packer has run.
			RectI frame;

			// Packed position and size.
			// This won't be set until after the packer has run.
			RectI packed;

			Entry(u64 id, const RectI& frame)
				: memory_index(0)
				, id(id)
				, page(0)
				, empty(true)
				, frame(frame)
				, packed(0, 0, 0, 0) {}
		};

		// maximum width / height of the generated texture
		int max_size;

		// whether the generated texture must be a power of 2
		bool power_of_two;

		// spacing between each packed subtexture
		int spacing;

		// padding on each subtexture (extrudes their borders outwards)
		int padding;

		// generated textures. There can be more than one if the packer was
		// unable to fit all of the provided subtextures into the max_size.
		Vector<Image> pages;

		Packer();
		Packer(int max_size, int spacing, bool power_of_two);
		Packer(const Packer&) = delete;
		Packer& operator=(const Packer&) = delete;
		Packer(Packer&& src) noexcept;
		Packer& operator=(Packer&& src) noexcept;
		~Packer();
		
		// add a new entry
		void add(u64 id, int width, int height, const Color* pixels);

		// add a new entry
		void add(u64 id, const Image& bitmap);

		// add a new entry
		void add(u64 id, const Image& bitmap, const RectI& source);

		// add a new entry
		void add(u64 id, const FilePath& path);

		// returns a vector of all the resulting entries
		const Vector<Entry>& entries() const;

		// perform the packing
		void pack();

		// clear the current packer data
		void clear();

		// dispose all resources used by the packer
		void dispose();

	private:
		struct Node
		{
			bool used;
			RectI rect;
			Node* right;
			Node* down;

			Node();
			Node* Find(int w, int h);
			Node* Reset(const RectI& rect);
		};

		// whether the packer has any changes that require it to run again
		bool m_dirty;

		// buffer of all the image data we will be packing
		BufferStream m_buffer;

		// Entries to pack & their resulting data
		Vector<Entry> m_entries;

		// adds a new entry
		void add_entry(u64 id, int w, int h, const Color* pixels, const RectI& source);
	};
}