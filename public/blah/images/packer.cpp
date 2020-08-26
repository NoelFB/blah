#include <blah/images/packer.h>
#include <algorithm>
#include <cstring>

using namespace Blah;

Packer::Packer()
	: max_size(8192), power_of_two(true), spacing(1), padding(1), dirty(false) { }

Packer::Packer(int max_size, int spacing, bool power_of_two)
	: max_size(max_size), power_of_two(power_of_two), spacing(spacing), padding(1), dirty(false) { }

Packer::Packer(Packer&& src) noexcept
{
	max_size = src.max_size;
	power_of_two = src.power_of_two;
	spacing = src.spacing;
	padding = src.padding;
	dirty = src.dirty;
	pages = std::move(src.pages);
	entries = std::move(src.entries);
	buffer = std::move(src.buffer);
}

Packer& Packer::operator=(Packer&& src) noexcept
{
	max_size = src.max_size;
	power_of_two = src.power_of_two;
	spacing = src.spacing;
	padding = src.padding;
	dirty = src.dirty;
	pages = std::move(src.pages);
	entries = std::move(src.entries);
	buffer = std::move(src.buffer);
	return *this;
}

Packer::~Packer()
{
	dispose();
}

void Packer::add(uint64_t id, int width, int height, const Color* pixels)
{
	add_entry(id, width, height, pixels);
}

void Packer::add(uint64_t id, const Image& image)
{
	add_entry(id, image.width, image.height, image.pixels);
}

void Packer::add(uint64_t id, const String& path)
{
	add(id, Image(path.cstr()));
}

void Packer::add_entry(uint64_t id, int w, int h, const Color* pixels)
{
	dirty = true;

	Entry* entry = entries.expand(1);
	entry->id = id;
	entry->page = 0;
	entry->empty = true;
	entry->frame = RectI(0, 0, w, h);
	entry->packed = RectI(0, 0, 0, 0);

	// trim
	int top = 0, left = 0, right = w, bottom = h;

	// TOP:
	for (int y = 0; y < h; y++)
		for (int x = 0, s = y * w; x < w; x++, s++)
			if (pixels[s].a > 0)
			{
				top = y;
				goto JUMP_LEFT;
			}
	JUMP_LEFT:
	for (int x = 0; x < w; x++)
		for (int y = top, s = x + y * w; y < h; y++, s += w)
			if (pixels[s].a > 0)
			{
				left = x;
				goto JUMP_RIGHT;
			}
	JUMP_RIGHT:
	for (int x = w - 1; x >= left; x--)
		for (int y = top, s = x + y * w; y < h; y++, s += w)
			if (pixels[s].a > 0)
			{
				right = x + 1;
				goto JUMP_BOTTOM;
			}
	JUMP_BOTTOM:
	for (int y = h - 1; y >= top; y--)
		for (int x = left, s = x + y * w; x < right; x++, s++)
			if (pixels[s].a > 0)
			{
				bottom = y + 1;
				goto JUMP_END;
			}
	JUMP_END:;

	// pixels actually exist in this source
	if (right >= left && bottom >= top)
	{
		entry->empty = false;

		// store size
		entry->frame.x = -left;
		entry->frame.y = -top;
		entry->packed.w = (right - left);
		entry->packed.h = (bottom - top);

		// create pixel data
		entry->memoryIndex = buffer.position();

		// copy pixels over
		if (entry->packed.w == w && entry->packed.h == h)
		{
			buffer.write((char*)pixels, sizeof(Color) * w * h);
		}
		else
		{
			for (int i = 0; i < entry->packed.h; i++)
				buffer.write((char*)(pixels + left + (top + i) * entry->frame.w), sizeof(Color) * entry->packed.w);
		}
	}
}

void Packer::pack()
{
	if (!dirty)
		return;

	dirty = false;
	pages.clear();

	// only if we have stuff to pack
	auto count = entries.count();
	if (count > 0)
	{
		// get all the sources sorted largest -> smallest
		List<Entry*> sources;
		{
			sources.expand(count);
			int index = 0;

			for (int i = 0; i < entries.count(); i++)
				sources[index++] = &entries[i];

			std::sort(sources.begin(), sources.end(), [](Packer::Entry* a, Packer::Entry* b)
			{
				return a->packed.w * a->packed.h > b->packed.w * b->packed.h;
			});
		}

		// make sure the largest isn't too large
		if (sources[0]->packed.w + padding * 2 > max_size || sources[0]->packed.h + padding * 2 > max_size)
		{
			BLAH_ERROR("Source image is larger than max atlas size");
			return;
		}

		// we should never need more nodes than source images * 3
		// if this causes problems we could change it to use push_back I suppose
		List<Node> nodes;
		nodes.expand(count * 4);

		int packed = 0, page = 0;
		while (packed < count)
		{
			if (sources[packed]->empty)
			{
				packed++;
				continue;
			}

			int from = packed;
			int index = 0;
			Node* root = nodes[index++].Reset(RectI(0, 0, sources[from]->packed.w + padding * 2 + spacing, sources[from]->packed.h + padding * 2 + spacing));

			while (packed < count)
			{
				if (sources[packed]->empty)
				{
					packed++;
					continue;
				}

				int w = sources[packed]->packed.w + padding * 2 + spacing;
				int h = sources[packed]->packed.h + padding * 2 + spacing;

				Node* node = root->Find(w, h);

				// try to expand
				if (node == nullptr)
				{
					bool canGrowDown = (w <= root->rect.w) && (root->rect.h + h < max_size);
					bool canGrowRight = (h <= root->rect.h) && (root->rect.w + w < max_size);
					bool shouldGrowRight = canGrowRight && (root->rect.h >= (root->rect.w + w));
					bool shouldGrowDown = canGrowDown && (root->rect.w >= (root->rect.h + h));

					if (canGrowDown || canGrowRight)
					{
						// grow right
						if (shouldGrowRight || (!shouldGrowDown && canGrowRight))
						{
							Node* next = nodes[index++].Reset(RectI(0, 0, root->rect.w + w, root->rect.h));
							next->used = true;
							next->down = root;
							next->right = node = nodes[index++].Reset(RectI(root->rect.w, 0, w, root->rect.h));
							root = next;
						}
						// grow down
						else
						{
							Node* next = nodes[index++].Reset(RectI(0, 0, root->rect.w, root->rect.h + h));
							next->used = true;
							next->down = node = nodes[index++].Reset(RectI(0, root->rect.h, root->rect.w, h));
							next->right = root;
							root = next;
						}
					}
				}

				// doesn't fit
				if (node == nullptr)
					break;

				// add
				node->used = true;
				node->down = nodes[index++].Reset(RectI(node->rect.x, node->rect.y + h, node->rect.w, node->rect.h - h));
				node->right = nodes[index++].Reset(RectI(node->rect.x + w, node->rect.y, node->rect.w - w, h));

				sources[packed]->packed.x = node->rect.x + padding;
				sources[packed]->packed.y = node->rect.y + padding;
				packed++;
			}

			// get page size
			int pageWidth, pageHeight;
			if (power_of_two)
			{
				pageWidth = 2;
				pageHeight = 2;
				while (pageWidth < root->rect.w)
					pageWidth *= 2;
				while (pageHeight < root->rect.h)
					pageHeight *= 2;
			}
			else
			{
				pageWidth = root->rect.w;
				pageHeight = root->rect.h;
			}

			// create each page
			{
				pages.add(Image(pageWidth, pageHeight));

				// copy image data to image
				for (int i = from; i < packed; i++)
				{
					sources[i]->page = page;
					if (!sources[i]->empty)
					{
						RectI dst = sources[i]->packed;
						Color* src = (Color*)(buffer.data() + sources[i]->memoryIndex);

						// TODO:
						// Optimize this?
						if (padding > 0)
						{
							pages[page].set_pixels(RectI(dst.x - padding, dst.y, dst.w, dst.h), src);
							pages[page].set_pixels(RectI(dst.x + padding, dst.y, dst.w, dst.h), src);
							pages[page].set_pixels(RectI(dst.x, dst.y - padding, dst.w, dst.h), src);
							pages[page].set_pixels(RectI(dst.x, dst.y + padding, dst.w, dst.h), src);
						}

						pages[page].set_pixels(dst, src);

					}
				}
			}

			page++;
		}
	}
}

void Packer::clear()
{
	pages.clear();
	entries.clear();
	dirty = false;
}

void Packer::dispose()
{
	pages.dispose();
	entries.dispose();
	buffer.close();
	max_size = 0;
	power_of_two = 0;
	spacing = 0;
	dirty = false;
}

Packer::Node::Node()
	: used(false), rect(0, 0, 0, 0), right(nullptr), down(nullptr) { }

Packer::Node* Packer::Node::Find(int w, int h)
{
	if (used)
	{
		Packer::Node* r = right->Find(w, h);
		if (r != nullptr)
			return r;
		return down->Find(w, h);
	}
	else if (w <= rect.w && h <= rect.h)
		return this;
	return nullptr;
}

Packer::Node* Packer::Node::Reset(const RectI& rect)
{
	used = false;
	this->rect = rect;
	right = nullptr;
	down = nullptr;
	return this;
}