#include <blah_packer.h>
#include <algorithm>

using namespace Blah;

Packer::Packer()
	: max_size(8192), power_of_two(true), spacing(1), padding(1), m_dirty(false) { }

Packer::Packer(int max_size, int spacing, bool power_of_two)
	: max_size(max_size), power_of_two(power_of_two), spacing(spacing), padding(1), m_dirty(false) { }

void Packer::add(u64 id, int width, int height, const Color* pixels)
{
	add_entry(id, width, height, pixels, Recti(0, 0, width, height));
}

void Packer::add(u64 id, const Image& image)
{
	add_entry(id, image.width, image.height, image.pixels, Recti(0, 0, image.width, image.height));
}

void Packer::add(u64 id, const Image& image, const Recti& source)
{
	add_entry(id, image.width, image.height, image.pixels, source);
}

void Packer::add(u64 id, const FilePath& path)
{
	add(id, Image(path.cstr()));
}

void Packer::add_entry(u64 id, int w, int h, const Color* pixels, const Recti& source)
{
	m_dirty = true;

	Entry entry(id, Recti(0, 0, source.w, source.h));

	// trim
	int top = source.y, left = source.x, right = source.x, bottom = source.y;

	// TOP:
	for (int y = source.y; y < source.y + source.h; y++)
		for (int x = source.x, s = y * w; x < source.x + source.w; x++, s++)
			if (pixels[s].a > 0)
			{
				top = y;
				goto JUMP_LEFT;
			}
JUMP_LEFT:
	for (int x = source.x; x < source.x + source.w; x++)
		for (int y = top, s = x + y * w; y < source.y + source.h; y++, s += w)
			if (pixels[s].a > 0)
			{
				left = x;
				goto JUMP_RIGHT;
			}
JUMP_RIGHT:
	for (int x = source.x + source.w - 1; x >= left; x--)
		for (int y = top, s = x + y * w; y < source.y + source.h; y++, s += w)
			if (pixels[s].a > 0)
			{
				right = x + 1;
				goto JUMP_BOTTOM;
			}
JUMP_BOTTOM:
	for (int y = source.y + source.h - 1; y >= top; y--)
		for (int x = left, s = x + y * w; x < right; x++, s++)
			if (pixels[s].a > 0)
			{
				bottom = y + 1;
				goto JUMP_END;
			}
JUMP_END:;

	// pixels actually exist in this source
	if (right > left && bottom > top)
	{
		entry.empty = false;

		// store size
		entry.frame.x = source.x - left;
		entry.frame.y = source.y - top;
		entry.packed.w = (right - left);
		entry.packed.h = (bottom - top);

		// create pixel data
		entry.memory_index = m_buffer.position();

		// copy pixels over
		if (entry.packed.w == w && entry.packed.h == h)
		{
			m_buffer.write((char*)pixels, sizeof(Color) * w * h);
		}
		else
		{
			for (int i = 0; i < entry.packed.h; i++)
				m_buffer.write((char*)(pixels + left + (top + i) * w), sizeof(Color) * entry.packed.w);
		}
	}

	m_entries.push_back(entry);
}

const Vector<Packer::Entry>& Packer::entries() const
{
	return m_entries;
}

void Packer::pack()
{
	if (!m_dirty)
		return;

	m_dirty = false;
	pages.clear();

	// only if we have stuff to pack
	auto count = m_entries.size();
	if (count > 0)
	{
		// get all the sources sorted largest -> smallest
		Vector<Entry*> sources;
		{
			sources.resize(count);
			int index = 0;

			for (int i = 0; i < m_entries.size(); i++)
				sources[index++] = &m_entries[i];

			std::sort(sources.begin(), sources.end(), [](Packer::Entry* a, Packer::Entry* b)
			{
				return a->packed.w * a->packed.h > b->packed.w * b->packed.h;
			});
		}

		// make sure the largest isn't too large
		if (sources[0]->packed.w + padding * 2 > max_size || sources[0]->packed.h + padding * 2 > max_size)
		{
			BLAH_ASSERT(false, "Source image is larger than max atlas size");
			return;
		}

		// we should never need more nodes than source images * 3
		// if this causes problems we could change it to use push_back I suppose
		Vector<Node> nodes;
		nodes.resize(count * 4);

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
			Node* root = nodes[index++].reset(Recti(0, 0, sources[from]->packed.w + padding * 2 + spacing, sources[from]->packed.h + padding * 2 + spacing));

			while (packed < count)
			{
				if (sources[packed]->empty)
				{
					packed++;
					continue;
				}

				int w = sources[packed]->packed.w + padding * 2 + spacing;
				int h = sources[packed]->packed.h + padding * 2 + spacing;

				Node* node = root->find(w, h);

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
							Node* next = nodes[index++].reset(Recti(0, 0, root->rect.w + w, root->rect.h));
							next->used = true;
							next->down = root;
							next->right = node = nodes[index++].reset(Recti(root->rect.w, 0, w, root->rect.h));
							root = next;
						}
						// grow down
						else
						{
							Node* next = nodes[index++].reset(Recti(0, 0, root->rect.w, root->rect.h + h));
							next->used = true;
							next->down = node = nodes[index++].reset(Recti(0, root->rect.h, root->rect.w, h));
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
				node->down = nodes[index++].reset(Recti(node->rect.x, node->rect.y + h, node->rect.w, node->rect.h - h));
				node->right = nodes[index++].reset(Recti(node->rect.x + w, node->rect.y, node->rect.w - w, h));

				sources[packed]->packed.x = node->rect.x + padding;
				sources[packed]->packed.y = node->rect.y + padding;
				packed++;
			}

			// get page size
			int page_width, page_height;
			if (power_of_two)
			{
				page_width = 2;
				page_height = 2;
				while (page_width < root->rect.w)
					page_width *= 2;
				while (page_height < root->rect.h)
					page_height *= 2;
			}
			else
			{
				page_width = root->rect.w;
				page_height = root->rect.h;
			}

			// create each page
			{
				pages.emplace_back(page_width, page_height);

				// copy image data to image
				for (int i = from; i < packed; i++)
				{
					sources[i]->page = page;
					if (!sources[i]->empty)
					{
						Recti dst = sources[i]->packed;
						Color* src = (Color*)(m_buffer.data() + sources[i]->memory_index);

						// TODO:
						// Optimize this?
						if (padding > 0)
						{
							Image& image = pages[page];
							for (int x = -padding; x < dst.w + padding; x++)
								for (int y = -padding; y < dst.h + padding; y++)
								{
									int sx = (x < 0 ? 0 : (x > dst.w - 1 ? dst.w - 1 : x));
									int sy = (y < 0 ? 0 : (y > dst.h - 1 ? dst.h - 1 : y));
									image.pixels[dst.x + x + (dst.y + y) * image.width] = src[sx + sy * dst.w];
								}
						}
						else
						{
							pages[page].set_pixels(dst, src);
						}
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
	m_entries.clear();
	m_buffer.clear();
	m_dirty = false;
}

Packer::Node::Node()
	: used(false), rect(0, 0, 0, 0), right(nullptr), down(nullptr) { }

Packer::Node* Packer::Node::find(int w, int h)
{
	if (used)
	{
		Packer::Node* r = right->find(w, h);
		if (r != nullptr)
			return r;
		return down->find(w, h);
	}
	else if (w <= rect.w && h <= rect.h)
		return this;
	return nullptr;
}

Packer::Node* Packer::Node::reset(const Recti& rect)
{
	used = false;
	this->rect = rect;
	right = nullptr;
	down = nullptr;
	return this;
}