#pragma once
#include <memory>

namespace Blah
{
	struct Config
	{
		const char* name;
		int width;
		int height;
		int max_updates;
		int target_framerate;

		void (*on_startup)();
		void (*on_shutdown)();
		void (*on_update)();
		void (*on_render)();
		void (*on_exit_request)();

		void (*on_info)(const char* text);
		void (*on_warn)(const char* text);
		void (*on_error)(const char* text);

		Config();
	};

	enum class Renderer
	{
		None = -1,
		OpenGL,
		D3D11,
		Metal,
		Count
	};

	struct RendererFeatures
	{
		bool instancing = false;
		bool origin_bottom_left = false;
		int max_texture_size = 0;
	};

	class FrameBuffer;
	using FrameBufferRef = std::shared_ptr<FrameBuffer>;

	namespace App
	{
		// Runs the application
		bool run(const Config* config);

		// Returns whether the application is running
		bool is_running();

		// Exits the application
		void exit();

		// Gets the config data used to run the application
		const Config* config();

		// Gets the working path
		const char* path();

		// Gets the user path
		const char* user_path();

		// Gets the width of the window
		int width();

		// Gets the height of the window
		int height();
		
		// Gets the drawable width of the window
		int draw_width();

		// Gets the drawable height of the window
		int draw_height();

		// Gets the content scale based on the OS
		float content_scale();

		// Toggles fullscreen
		void fullscreen(bool enabled);

		// Returns the Rendering API in use
		Renderer renderer();

		// Retrieves the Renderer Features
		const RendererFeatures& renderer_features();

		// Reference to the window's back buffer
		extern const FrameBufferRef backbuffer;
	}
}