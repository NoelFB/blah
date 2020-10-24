#pragma once
#include <blah/math/color.h>
#include <blah/images/image.h>
#include <blah/containers/str.h>
#include <blah/streams/stream.h>

namespace Blah
{
	class Aseprite
	{
	public:

		enum class Modes
		{
			Indexed = 1,
			Grayscale = 2,
			RGBA = 4
		};

		enum class Chunks
		{
			OldPaletteA = 0x0004,
			OldPaletteB = 0x0011,
			Layer = 0x2004,
			Cel = 0x2005,
			CelExtra = 0x2006,
			Mask = 0x2016,
			Path = 0x2017,
			FrameTags = 0x2018,
			Palette = 0x2019,
			UserData = 0x2020,
			Slice = 0x2022
		};

		enum class LoopDirections
		{
			Forward = 0,
			Reverse = 1,
			PingPong = 2
		};

		enum class LayerFlags
		{
			Visible = 1,
			Editable = 2,
			LockMovement = 4,
			Background = 8,
			PreferLinkedCels = 16,
			Collapsed = 32,
			Reference = 64
		};

		enum class LayerTypes
		{
			Normal = 0,
			Group = 1
		};

		struct UserData
		{
			String text;
			Color color;
		};

		struct Layer;

		struct Cel
		{
			int layer_index = 0;
			int linked_frame_index = 0;
			int x = 0;
			int y = 0;
			unsigned char alpha = 0;
			Image image;
			UserData userdata;
		};

		struct Frame
		{
			int duration = 0;
			Image image;
			Vector<Cel> cels;
		};

		struct Layer
		{
			LayerFlags flag = (LayerFlags)0;
			LayerTypes type = LayerTypes::Normal;
			String name;
			int child_level = 0;
			int blendmode = 0;
			unsigned char alpha = 0;
			bool visible = true;
			UserData userdata;
		};

		struct Tag
		{
			String name;
			LoopDirections loops = LoopDirections::Forward;
			int from = 0;
			int to = 0;
			Color color;
			UserData userdata;
		};

		struct Slice
		{
			int frame = 0;
			String name;
			Point origin;
			int width = 0;
			int height = 0;
			bool has_pivot = false;
			Point pivot;
			UserData userdata;
		};

		Modes mode = Modes::RGBA;
		int width = 0;
		int height = 0;

		Vector<Layer> layers;
		Vector<Frame> frames;
		Vector<Tag> tags;
		Vector<Slice> slices;
		Vector<Color> palette;

		Aseprite();
		Aseprite(const char* path);
		Aseprite(Stream& stream);
		Aseprite(const Aseprite& src);
		Aseprite(Aseprite&& src) noexcept;
		Aseprite& operator=(const Aseprite& src);
		Aseprite& operator=(Aseprite&& src) noexcept;
		~Aseprite();

	private:
		UserData* last_userdata = nullptr;

		void Parse(Stream& stream);
		void ParseLayer(Stream& stream, int frame);
		void ParseCel(Stream& stream, int frame, size_t maxPosition);
		void ParsePalette(Stream& stream, int frame);
		void ParseUserData(Stream& stream, int frame);
		void ParseTag(Stream& stream, int frame);
		void ParseSlice(Stream& stream, int frame);
		void RenderCel(Cel* cel, Frame* frame);
	};
}