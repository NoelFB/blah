#pragma once
#include <blah/graphics/graphics.h>

namespace Blah
{
	namespace Internal
	{
		class GraphicsDevice;

		// graphics device metadata used to instantiate and destroy
		// devices of specific apis
		struct GraphicsDeviceInfo
		{
			GfxAPI api;
			bool (*supported)();
			GraphicsDevice* (*create)();
			void (*destroy)(GraphicsDevice*);
		};

		// graphics implementations
		extern GraphicsDeviceInfo OpenGL_DeviceInfo;

		// graphics device
		// each graphics implementation needs to implement this
		// ex. one for opengl, one for vulkan, etc
		class GraphicsDevice
		{
		public:
			bool valid = false;
			GraphicsInfo info;

			virtual ~GraphicsDevice() = default;
			virtual void startup() = 0;
			virtual void update() = 0;
			virtual void shutdown() = 0;
			virtual void before_render() = 0;
			virtual void after_render() = 0;

			virtual TextureRef create_texture(int width, int height, TextureFilter filter, TextureWrap wrap_x, TextureWrap wrap_y, TextureFormat format) = 0;
			virtual FrameBufferRef create_framebuffer(int width, int height, const TextureFormat* attachments, int attachmentCount) = 0;
			virtual ShaderRef create_shader(const ShaderData* data) = 0;
			virtual MeshRef create_mesh() = 0;
			virtual void render(RenderCall* call) = 0;
			virtual void clear(const FrameBufferRef& target, uint32_t rgba) = 0;
		};

		namespace Graphics
		{
			// Picks a graphics API based on the available APIs
			GfxAPI pick_api();

			// Initializes the Graphics with the given API
			bool init(GfxAPI api);

			// Is called internally by the Application every update
			void frame();

			// Is called internally by the Application before every render
			void before_render();

			// Is called internally by the Application after every render
			void after_render();

			// Called when the Application is shutting down
			void shutdown();
		}
	}
}