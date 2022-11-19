#pragma once
#include <blah_common.h>
#include <blah_image.h>
#include <blah_color.h>
#include <blah_spatial.h>
#include <blah_string.h>
#include <blah_vector.h>
#include <blah_stream.h>
#include <blah_filesystem.h>

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
			Recti frame;

			// Packed position and size.
			// This won't be set until after the packer has run.
			Recti packed;

			Entry(u64 id, const Recti& frame)
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
		// unable to fit all the provided subtextures into the max_size.
		Vector<Image> pages;

		Packer();
		Packer(int max_size, int spacing, bool power_of_two);
		
		// add a new entry
		void add(u64 id, int width, int height, const Color* pixels);

		// add a new entry
		void add(u64 id, const Image& bitmap);

		// add a new entry
		void add(u64 id, const Image& bitmap, const Recti& source);

		// add a new entry
		void add(u64 id, const FilePath& path);

		// returns a vector of all the resulting entries
		const Vector<Entry>& entries() const;

		// perform the packing
		void pack();

		// clear the current packer data
		void clear();

	private:
		struct Node
		{
			bool used;
			Recti rect;
			Node* right;
			Node* down;

			Node();
			Node* find(int w, int h);
			Node* reset(const Recti& rect);
		};

		// whether the packer has any changes that require it to run again
		bool m_dirty;

		// buffer of all the image data we will be packing
		BufferStream m_buffer;

		// Entries to pack & their resulting data
		Vector<Entry> m_entries;

		// adds a new entry
		void add_entry(u64 id, int w, int h, const Color* pixels, const Recti& source);
	};
}