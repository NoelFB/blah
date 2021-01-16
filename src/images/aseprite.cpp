#include <blah/images/aseprite.h>
#include <blah/streams/filestream.h>
#include <blah/core/filesystem.h>
#include <blah/core/log.h>

#define STBI_NO_STDIO
#define STBI_ONLY_ZLIB
#include "../third_party/stb_image.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MUL_UN8(a, b, t) \
	((t) = (a) * (uint16_t)(b) + 0x80, ((((t) >> 8) + (t) ) >> 8))

using namespace Blah;

Aseprite::Aseprite()
{

}

Aseprite::Aseprite(const char* path)
{
	FileStream fs(path, FileMode::Read);
	parse(fs);
}

Aseprite::Aseprite(Stream& stream)
{
	parse(stream);
}

Aseprite::Aseprite(const Aseprite& src)
{
	mode = src.mode;
	width = src.width;
	height = src.height;
	layers = src.layers;
	frames = src.frames;
	tags = src.tags;
	slices = src.slices;
	palette = src.palette;
}

Aseprite::Aseprite(Aseprite&& src) noexcept
{
	mode = src.mode;
	width = src.width;
	height = src.height;
	layers = std::move(src.layers);
	frames = std::move(src.frames);
	tags = std::move(src.tags);
	slices = std::move(src.slices);
	palette = std::move(src.palette);
}

Aseprite& Aseprite::operator=(const Aseprite& src)
{
	mode = src.mode;
	width = src.width;
	height = src.height;
	layers = src.layers;
	frames = src.frames;
	tags = src.tags;
	slices = src.slices;
	palette = src.palette;
	return *this;
}

Aseprite& Aseprite::operator=(Aseprite&& src) noexcept
{
	mode = src.mode;
	width = src.width;
	height = src.height;
	layers = std::move(src.layers);
	frames = std::move(src.frames);
	tags = std::move(src.tags);
	slices = std::move(src.slices);
	palette = std::move(src.palette);
	return *this;
}

Aseprite::~Aseprite()
{

}

void Aseprite::parse(Stream& stream)
{
	if (!stream.is_readable())
	{
		BLAH_ERROR("Stream is not readable");
		return;
	}

	int frame_count = 0;

	// header
	{
		// filesize
		stream.read<uint32_t>(Endian::Little);

		// magic number
		auto magic = stream.read<uint16_t>(Endian::Little);
		if (magic != 0xA5E0)
		{
			BLAH_ERROR("File is not a valid Aseprite file");
			return;
		}

		// main info
		frame_count = stream.read<uint16_t>(Endian::Little);
		width = stream.read<uint16_t>(Endian::Little);
		height = stream.read<uint16_t>(Endian::Little);
		mode = static_cast<Aseprite::Modes>(stream.read<uint16_t>(Endian::Little) / 8);

		// don't care about other info
		stream.read<uint32_t>(Endian::Little);		// Flags
		stream.read<uint16_t>(Endian::Little);		// Speed (deprecated)
		stream.read<uint32_t>(Endian::Little);		// Should be 0
		stream.read<uint32_t>(Endian::Little);		// Should be 0
		stream.read<uint8_t>(Endian::Little);		// Palette entry
		stream.seek(stream.position() + 3);			// Ignore these bytes
		stream.read<uint16_t>(Endian::Little);		// Number of colors (0 means 256 for old sprites)
		stream.read<int8_t>(Endian::Little);		// Pixel width
		stream.read<int8_t>(Endian::Little);		// Pixel height
		stream.seek(stream.position() + 92);		// For Future
	}

	frames.resize(frame_count);

	// frames
	for (int i = 0; i < frame_count; i++)
	{
		auto frameStart = stream.position();
		auto frameEnd = frameStart + stream.read<uint32_t>(Endian::Little);
		unsigned int chunks = 0;

		// frame header
		{
			auto magic = stream.read<uint16_t>(Endian::Little); // magic number
			if (magic != 0xF1FA)
			{
				BLAH_ERROR("File is not a valid Aseprite file");
				return;
			}

			auto old_chunk_count = stream.read<uint16_t>(Endian::Little);
			frames[i].duration = stream.read<uint16_t>(Endian::Little);
			stream.seek(stream.position() + 2); // for future
			auto new_chunk_count = stream.read<uint32_t>(Endian::Little);

			if (old_chunk_count == 0xFFFF)
				chunks = new_chunk_count;
			else
				chunks = old_chunk_count;
		}

		// make frame image
		frames[i].image = Image(width, height);

		// frame chunks
		for (unsigned int j = 0; j < chunks; j++)
		{
			auto chunkStart = stream.position();
			auto chunkEnd = chunkStart + stream.read<uint32_t>(Endian::Little);
			auto chunkType = static_cast<Chunks>(stream.read<uint16_t>(Endian::Little));

			switch (chunkType)
			{
			case Chunks::Layer: parse_layer(stream, i); break;
			case Chunks::Cel: parse_cel(stream, i, chunkEnd); break;
			case Chunks::Palette: parse_palette(stream, i); break;
			case Chunks::UserData: parse_user_data(stream, i); break;
			case Chunks::FrameTags: parse_tag(stream, i); break;
			case Chunks::Slice: parse_slice(stream, i); break;
			default: break;
			}

			stream.seek(chunkEnd);
		}

		stream.seek(frameEnd);
	}
}

void Aseprite::parse_layer(Stream& stream, int frame)
{
	layers.emplace_back();

	auto& layer = layers.back();
	layer.flag = static_cast<LayerFlags>(stream.read<uint16_t>(Endian::Little));
	layer.visible = ((int)layer.flag & (int)LayerFlags::Visible) == (int)LayerFlags::Visible;
	layer.type = static_cast<LayerTypes>(stream.read<uint16_t>(Endian::Little));
	layer.child_level = stream.read<uint16_t>(Endian::Little);
	stream.read<uint16_t>(Endian::Little); // width
	stream.read<uint16_t>(Endian::Little); // height
	layer.blendmode = stream.read<uint16_t>(Endian::Little);
	layer.alpha = stream.read<uint8_t>(Endian::Little);
	stream.seek(stream.position() + 3); // for future

	layer.name.set_length(stream.read<uint16_t>(Endian::Little));
	stream.read(layer.name.cstr(), layer.name.length());

	layer.userdata.color = 0xffffff;
	layer.userdata.text = "";
	m_last_userdata = &(layer.userdata);
}

void Aseprite::parse_cel(Stream& stream, int frameIndex, size_t maxPosition)
{
	Frame& frame = frames[frameIndex];

	frame.cels.emplace_back();
	auto& cel = frame.cels.back();
	cel.layer_index = stream.read<uint16_t>(Endian::Little);
	cel.x = stream.read<uint16_t>(Endian::Little);
	cel.y = stream.read<uint16_t>(Endian::Little);
	cel.alpha = stream.read<uint8_t>(Endian::Little);
	cel.linked_frame_index = -1;

	auto celType = stream.read<uint16_t>(Endian::Little);
	stream.seek(stream.position() + 7);

	// RAW or DEFLATE
	if (celType == 0 || celType == 2)
	{
		auto width = stream.read<uint16_t>(Endian::Little);
		auto height = stream.read<uint16_t>(Endian::Little);
		auto count = width * height * (int)mode;

		cel.image = Image(width, height);

		// RAW
		if (celType == 0)
		{
			stream.read(cel.image.pixels, count);
		}
		// DEFLATE (zlib)
		else
		{
			// this could be optimized to use a buffer on the stack if we only read set chunks at a time
			// stbi's zlib doesn't have that functionality though
			auto size = maxPosition - stream.position();
			if (size > INT32_MAX)
				size = INT32_MAX;

			char* buffer = new char[size];
			stream.read(buffer, size);

			int olen = width * height * sizeof(Color);
			int res = stbi_zlib_decode_buffer((char*)cel.image.pixels, olen, buffer, (int)size);

			delete[] buffer;

			if (res < 0)
			{
				BLAH_ERROR("Unable to parse Aseprite file");
				return;
			}
		}

		// convert to pixels
		// note: we work in-place to save having to store stuff in a buffer
		if (mode == Modes::Grayscale)
		{
			auto src = (unsigned char*)cel.image.pixels;
			auto dst = cel.image.pixels;
			for (int d = width * height - 1, s = (width * height - 1) * 2; d >= 0; d--, s -= 2)
				dst[d] = Color(src[s], src[s], src[s], src[s + 1]);
		}
		else if (mode == Modes::Indexed)
		{
			auto src = (unsigned char*)cel.image.pixels;
			auto dst = cel.image.pixels;
			for (int i = width * height - 1; i >= 0; i--)
				dst[i] = palette[src[i]];
		}

	}
	// REFERENCE
	// this cel directly references a previous cel
	else if (celType == 1)
	{
		cel.linked_frame_index = stream.read<uint16_t>(Endian::Little);
	}

	// draw to frame if visible
	if ((int)layers[cel.layer_index].flag & (int)LayerFlags::Visible)
	{
		render_cel(&cel, &frame);
	}

	cel.userdata.color = 0xffffff;
	cel.userdata.text = "";
	m_last_userdata = &(cel.userdata);
}

void Aseprite::parse_palette(Stream& stream, int frame)
{
	/* size */ stream.read<uint32_t>(Endian::Little);
	auto start = stream.read<uint32_t>(Endian::Little);
	auto end = stream.read<uint32_t>(Endian::Little);
	stream.seek(stream.position() + 8);

	palette.resize(palette.size() + (end - start + 1));

	for (int p = 0, len = static_cast<int>(end - start) + 1; p < len; p++)
	{
		auto hasName = stream.read<uint16_t>(Endian::Little);
		palette[start + p] = stream.read<uint32_t>(Endian::Little);

		if (hasName & 0xF000)
		{
			int len = stream.read<uint16_t>(Endian::Little);
			stream.seek(stream.position() + len);
		}
	}
}

void Aseprite::parse_user_data(Stream& stream, int frame)
{
	if (m_last_userdata != nullptr)
	{
		auto flags = stream.read<uint32_t>(Endian::Little);

		// has text
		if (flags & (1 << 0))
		{
			m_last_userdata->text.set_length(stream.read<uint16_t>(Endian::Little));
			stream.read(m_last_userdata->text.cstr(), m_last_userdata->text.length());
		}

		// has color
		if (flags & (1 << 1))
			m_last_userdata->color = stream.read<uint32_t>(Endian::Little);
	}
}

void Aseprite::parse_tag(Stream& stream, int frame)
{
	auto count = stream.read<uint16_t>(Endian::Little);
	stream.seek(stream.position() + 8);

	for (int t = 0; t < count; t++)
	{
		Tag tag;
		tag.from = stream.read<uint16_t>(Endian::Little);
		tag.to = stream.read<uint16_t>(Endian::Little);
		tag.loops = static_cast<LoopDirections>(stream.read<int8_t>(Endian::Little));

		stream.seek(stream.position() + 8);
		tag.color = Color(stream.read<int8_t>(), stream.read<int8_t>(), stream.read<int8_t>(Endian::Little), 255);
		stream.seek(stream.position() + 1);

		tag.name.set_length(stream.read<uint16_t>(Endian::Little));
		stream.read(tag.name.cstr(), tag.name.length());

		tags.push_back(tag);
	}
}

void Aseprite::parse_slice(Stream& stream, int frame)
{
	int count = stream.read<uint32_t>(Endian::Little);
	int flags = stream.read<uint32_t>(Endian::Little);
	stream.read<uint32_t>(Endian::Little); // reserved

	String name;
	name.set_length(stream.read<uint16_t>(Endian::Little));
	stream.read(name.cstr(), name.length());

	for (int s = 0; s < count; s++)
	{
		slices.emplace_back();

		auto& slice = slices.back();
		slice.name = name;
		slice.frame = stream.read<uint32_t>(Endian::Little);
		slice.origin.x = stream.read<int32_t>(Endian::Little);
		slice.origin.y = stream.read<int32_t>(Endian::Little);
		slice.width = stream.read<uint32_t>(Endian::Little);
		slice.height = stream.read<uint32_t>(Endian::Little);

		// 9 slice (ignored atm)
		if (flags & (1 << 0))
		{
			stream.read<int32_t>(Endian::Little);
			stream.read<int32_t>(Endian::Little);
			stream.read<uint32_t>(Endian::Little);
			stream.read<uint32_t>(Endian::Little);
		}

		// pivot point
		slice.has_pivot = false;
		if (flags & (1 << 1))
		{
			slice.has_pivot = true;
			slice.pivot.x = stream.read<uint32_t>(Endian::Little);
			slice.pivot.y = stream.read<uint32_t>(Endian::Little);
		}

		slice.userdata.color = 0xffffff;
		slice.userdata.text = "";
		m_last_userdata = &(slice.userdata);
	}
}

void Aseprite::render_cel(Cel* cel, Frame* frame)
{
	Layer& layer = layers[cel->layer_index];

	while (cel->linked_frame_index >= 0)
	{
		auto& frame = frames[cel->linked_frame_index];
		for (auto& it : frame.cels)
			if (it.layer_index == cel->layer_index)
			{
				cel = &it;
				break;
			}
	}

	int t;
	unsigned char opacity = MUL_UN8(cel->alpha, layer.alpha, t);
	if (opacity <= 0)
		return;

	auto src = cel->image.pixels;
	auto srcX = cel->x;
	auto srcY = cel->y;
	auto srcW = cel->image.width;
	auto srcH = cel->image.height;
	auto dst = frame->image.pixels;
	auto dstW = frame->image.width;
	auto dstH = frame->image.height;

	// blit pixels
	int left = MAX(0, srcX);
	int right = MIN(dstW, srcX + srcW);
	int top = MAX(0, srcY);
	int bottom = MIN(dstH, srcY + srcH);

	if (layer.blendmode == 0)
	{
		for (int dx = left, sx = -MIN(srcX, 0); dx < right; dx++, sx++)
		{
			for (int dy = top, sy = -MIN(srcY, 0); dy < bottom; dy++, sy++)
			{
				Color* srcColor = (src + sx + sy * srcW);
				Color* dstColor = (dst + dx + dy * dstW);

				if (srcColor->a != 0)
				{
					auto sa = MUL_UN8(srcColor->a, opacity, t);
					auto ra = dstColor->a + sa - MUL_UN8(dstColor->a, sa, t);

					dstColor->r = (unsigned char)(dstColor->r + (srcColor->r - dstColor->r) * sa / ra);
					dstColor->g = (unsigned char)(dstColor->g + (srcColor->g - dstColor->g) * sa / ra);
					dstColor->b = (unsigned char)(dstColor->b + (srcColor->b - dstColor->b) * sa / ra);
					dstColor->a = (unsigned char)ra;
				}
			}
		}
	}
	else
	{
		BLAH_ERROR("Aseprite blendmodes aren't implemented");
	}
}
