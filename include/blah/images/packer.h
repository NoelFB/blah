#pragma once
#include <blah/images/image.h>
#include <blah/math/color.h>
#include <blah/math/rectI.h>
#include <blah/math/point.h>
#include <blah/containers/str.h>
#include <blah/containers/vector.h>
#include <blah/streams/bufferstream.h>

namespace Blah
{
	// Texture Packer, which takes source images and combines
	// them into a single large texture.
	class Packer
	{
	public:
		class Entry
		{
		friend class Packer;
		private:
			i64 memory_index;
		public:
			u64 id;
			int page;
			bool empty;
			RectI frame;
			RectI packed;

			Entry(u64 id, const RectI& frame)
				: memory_index(0), id(id), page(0), empty(true), frame(frame), packed(0, 0, 0, 0) {}
		};

		int max_size;
		bool power_of_two;
		int spacing;
		int padding;

		Vector<Image> pages;
		Vector<Entry> entries;

		Packer();
		Packer(int max_size, int spacing, bool power_of_two);
		Packer(const Packer&) = delete;
		Packer& operator=(const Packer&) = delete;
		Packer(Packer&& src) noexcept;
		Packer& operator=(Packer&& src) noexcept;
		~Packer();
		
		void add(u64 id, int width, int height, const Color* pixels);
		void add(u64 id, const Image& bitmap);
		void add(u64 id, const String& path);

		void pack();
		void clear();
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

		bool m_dirty;
		BufferStream m_buffer;

		void add_entry(u64 id, int w, int h, const Color* pixels);
	};
}