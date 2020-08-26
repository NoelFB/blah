#pragma once
#include <blah/images/image.h>
#include <blah/math/color.h>
#include <blah/math/rectI.h>
#include <blah/math/point.h>
#include <blah/containers/list.h>
#include <blah/containers/str.h>
#include <blah/streams/bufferstream.h>

namespace Blah
{

	class Packer
	{
	public:
		class Entry
		{
		friend class Packer;
		private:
			int64_t memoryIndex = 0;
		public:
			int page = 0;
			uint64_t id;
			bool empty = true;
			RectI packed;
			RectI frame;
		};

		int max_size;
		bool power_of_two;
		int spacing;
		int padding;

		List<Image> pages;
		List<Entry> entries;

		Packer();
		Packer(int max_size, int spacing, bool power_of_two);
		Packer(const Packer&) = delete;
		Packer& operator=(const Packer&) = delete;
		Packer(Packer&& src) noexcept;
		Packer& operator=(Packer&& src) noexcept;
		~Packer();
		
		void add(uint64_t id, int width, int height, const Color* pixels);
		void add(uint64_t id, const Image& bitmap);
		void add(uint64_t id, const String& path);

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

		bool dirty;
		BufferStream buffer;

		void add_entry(uint64_t id, int w, int h, const Color* pixels);
	};
}