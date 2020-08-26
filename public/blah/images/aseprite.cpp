#include <blah/images/aseprite.h>
#include <blah/streams/filestream.h>

#define STBI_NO_STDIO
#define STBI_NO_JPEG
#define STBI_NO_PNG
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#include <blah/third_party/stb_image.h>

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
	Parse(fs);
}

Aseprite::Aseprite(Stream& stream)
{
	Parse(stream);
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

void Aseprite::Parse(Stream& stream)
{
	if (!stream.is_readable())
	{
		BLAH_ERROR("Stream is not readable");
		return;
	}

	int frameCount = 0;

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
		frameCount = stream.read<uint16_t>(Endian::Little);
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

	frames.expand(frameCount);

	// frames
	for (int i = 0; i < frameCount; i ++)
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
		for (unsigned int j = 0; j < chunks; j ++)
		{
			auto chunkStart = stream.position();
			auto chunkEnd = chunkStart + stream.read<uint32_t>(Endian::Little);
			auto chunkType = static_cast<Chunks>(stream.read<uint16_t>(Endian::Little));

			switch (chunkType)
			{
				case Chunks::Layer: ParseLayer(stream, i); break;
				case Chunks::Cel: ParseCel(stream, i, (size_t)chunkEnd); break;
				case Chunks::Palette: ParsePalette(stream, i); break;
				case Chunks::UserData: ParseUserData(stream, i); break;
				case Chunks::FrameTags: ParseTag(stream, i); break;
				case Chunks::Slice: ParseSlice(stream, i); break;
				default: break;
			}

			stream.seek(chunkEnd);
		}

		stream.seek(frameEnd);
	}
}

void Aseprite::ParseLayer(Stream& stream, int frame)
{
	auto layer = layers.expand(1);
	layer->flag = static_cast<LayerFlags>(stream.read<uint16_t>(Endian::Little));
	layer->visible = ((int)layer->flag & (int)LayerFlags::Visible) == (int)LayerFlags::Visible;
	layer->type = static_cast<LayerTypes>(stream.read<uint16_t>(Endian::Little));
	layer->child_level = stream.read<uint16_t>(Endian::Little);
	stream.read<uint16_t>(Endian::Little); // width
	stream.read<uint16_t>(Endian::Little); // height
	layer->blendmode = stream.read<uint16_t>(Endian::Little);
	layer->alpha = stream.read<uint8_t>(Endian::Little);
	stream.seek(stream.position() + 3); // for future

	layer->name.set_length(stream.read<uint16_t>(Endian::Little));
	stream.read(layer->name.cstr(), layer->name.length());

	layer->userdata.color = 0xffffff;
	layer->userdata.text = "";
	last_userdata = &(layer->userdata);
}

void Aseprite::ParseCel(Stream& stream, int frameIndex, size_t maxPosition)
{
	Frame& frame = frames[frameIndex];

	auto cel = frame.cels.expand(1);
	cel->layer_index = stream.read<uint16_t>(Endian::Little);
	cel->x = stream.read<uint16_t>(Endian::Little);
	cel->y = stream.read<uint16_t>(Endian::Little);
	cel->alpha = stream.read<uint8_t>(Endian::Little);
	cel->linked_frame_index = -1;
	cel->linked_cel_index = -1;

	auto celType = stream.read<uint16_t>(Endian::Little);
	stream.seek(stream.position() + 7);

	// RAW or DEFLATE
	if (celType == 0 || celType == 2)
	{
		auto width = stream.read<uint16_t>(Endian::Little);
		auto height = stream.read<uint16_t>(Endian::Little);
		auto count = width * height * (int)mode;

		cel->image = Image(width, height);

		// RAW
		if (celType == 0)
		{
			stream.read(cel->image.pixels, count);
		}
		// DEFLATE (zlib)
		else
		{
			// this could be optimized to use a buffer on the stack if we only read set chunks at a time
			// stbi's zlib doesn't have that functionality though
			auto size = maxPosition - stream.position();
			if (size > INT32_MAX)
				size = INT32_MAX;
			if (size < 0)
				size = 0;

			char* buffer = new char[size];
			stream.read(buffer, size);

			int olen = width * height * sizeof(Color);
			int res = stbi_zlib_decode_buffer((char*)cel->image.pixels, olen, buffer, (int)size);

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
			auto src = (unsigned char*)cel->image.pixels;
			auto dst = cel->image.pixels;
			for (int d = width * height - 1, s = (width * height - 1) * 2; d >= 0; d --, s -= 2)
				dst[d] = Color(src[s], src[s], src[s], src[s + 1]);
		}
		else if (mode == Modes::Indexed)
		{
			auto src = (unsigned char*)cel->image.pixels;
			auto dst = cel->image.pixels;
			for (int i = width * height - 1; i >= 0; i --)
				dst[i] = palette[src[i]];
		}

	}
	// REFERENCE
	// this cel directly references a previous cel
	else if (celType == 1)
	{
		cel->linked_frame_index = stream.read<uint16_t>(Endian::Little);
		cel->linked_cel_index = static_cast<int>(frame.cels.count() - 1);
	}

	// draw to frame if visible
	if ((int)layers[cel->layer_index].flag & (int)LayerFlags::Visible)
	{
		RenderCel(cel, &frame);
	}

	cel->userdata.color = 0xffffff;
	cel->userdata.text = "";
	last_userdata = &(cel->userdata);
}

void Aseprite::ParsePalette(Stream& stream, int frame)
{
	/* size */ stream.read<uint32_t>(Endian::Little);
	auto start = stream.read<uint32_t>(Endian::Little);
	auto end = stream.read<uint32_t>(Endian::Little);
	stream.seek(stream.position() + 8);

	palette.expand(palette.count() + (end - start + 1));

	for (int p = 0, len = static_cast<int>(end - start) + 1; p < len; p++)
	{
		auto hasName = stream.read<uint16_t>(Endian::Little);

		palette[start + p] = stream.read<Color>(Endian::Little);

		if (hasName & 0xF000)
		{
			int len = stream.read<uint16_t>(Endian::Little);
			stream.seek(stream.position() + len);
		}
	}	
}

void Aseprite::ParseUserData(Stream& stream, int frame)
{
	if (last_userdata != nullptr)
	{
		auto flags = stream.read<uint32_t>(Endian::Little);

		// has text
		if (flags & (1 << 0))
		{
			last_userdata->text.set_length(stream.read<uint16_t>(Endian::Little));
			stream.read(last_userdata->text.cstr(), last_userdata->text.length());
		}

		// has color
		if (flags & (1 << 1))
			last_userdata->color = stream.read<Color>(Endian::Little);
	}
}

void Aseprite::ParseTag(Stream& stream, int frame)
{
	auto count = stream.read<uint16_t>(Endian::Little);
	stream.seek(stream.position() + 8);

	for (int t = 0; t < count; t++)
	{
		auto tag = tags.expand(1);
		tag->from = stream.read<uint16_t>(Endian::Little);
		tag->to = stream.read<uint16_t>(Endian::Little);
		tag->loops = static_cast<LoopDirections>(stream.read<int8_t>(Endian::Little));

		stream.seek(stream.position() + 8);
		tag->color = Color(stream.read<int8_t>(), stream.read<int8_t>(), stream.read<int8_t>(Endian::Little), 255);
		stream.seek(stream.position() + 1);

		tag->name.set_length(stream.read<uint16_t>(Endian::Little));
		stream.read(tag->name .cstr(), tag->name.length());
	}
}

void Aseprite::ParseSlice(Stream& stream, int frame)
{
	int count = stream.read<uint32_t>(Endian::Little);
	int flags = stream.read<uint32_t>(Endian::Little);
	stream.read<uint32_t>(Endian::Little); // reserved

	String name;
	name.set_length(stream.read<uint16_t>(Endian::Little));
	stream.read(name.cstr(), name.length());

	for (int s = 0; s < count; s++)
	{
		auto slice = slices.expand(1);
		slice->name = name;
		slice->frame = stream.read<uint32_t>(Endian::Little);
		slice->origin.x = stream.read<int32_t>(Endian::Little);
		slice->origin.y = stream.read<int32_t>(Endian::Little);
		slice->width = stream.read<uint32_t>(Endian::Little);
		slice->height = stream.read<uint32_t>(Endian::Little);
		
		// 9 slice (ignored atm)
		if (flags & (1 << 0))
		{
			stream.read<int32_t>(Endian::Little);
			stream.read<int32_t>(Endian::Little);
			stream.read<uint32_t>(Endian::Little);
			stream.read<uint32_t>(Endian::Little);
		}

		// pivot point
		slice->has_pivot = false;
		if (flags & (1 << 1))
		{
			slice->has_pivot = true;
			slice->pivot.x = stream.read<uint32_t>(Endian::Little);
			slice->pivot.y = stream.read<uint32_t>(Endian::Little);
		}

		slice->userdata.color = 0xffffff;
		slice->userdata.text = "";
		last_userdata = &(slice->userdata);
	}
}

void Aseprite::RenderCel(Cel* cel, Frame* frame)
{
	Layer& layer = layers[cel->layer_index];
	
	while (cel->linked_frame_index >= 0)
		cel = &(frames[cel->linked_frame_index].cels[cel->linked_cel_index]);
	
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
