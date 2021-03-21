#pragma once
#include <memory>
#include <functional>
#include <blah/core/common.h>

namespace Blah
{
	// Application Event Functions
	using AppEventFn = std::function<void()>;

	// Application Logging Functions
	using AppLogFn = std::function<void(const char* message, Log::Category category)>;

	// Application Configuration
	struct Config
	{
		// Application name.
		// This has no default and must be set.
		const char* name;

		// Starting width, in pixels.
		// Depending on the OS DPI, the true window size may be a multiple of this.
		// This has no default and must be set.
		int width;

		// Starting height, in pixels.
		// Depending on the OS DPI, the true window size may be a multiple of this.
		// This has no default and must be set.
		int height;

		// maximum updates to run before "giving up" and reducing frame rate.
		// this avoids the 'spiral of death'.
		// defaults to 5.
		int max_updates;

		// target framerate.
		// defaults to 60.
		int target_framerate;

		// Callback on application startup
		// Defaults to nothing.
		AppEventFn on_startup;

		// Callback on application shutdown
		// Defaults to nothing.
		AppEventFn on_shutdown;

		// Callback on application update
		// Defaults to nothing.
		AppEventFn on_update;

		// Callback on application render
		// Defaults to nothing.
		AppEventFn on_render;

		// Callback when the user has requested the application close.
		// For example, pressing the Close button
		// By default this calls `App::exit()`
		AppEventFn on_exit_request;

		// Callback when the application logs info/warning/errors
		// Defaults to printf.
		AppLogFn on_log;

		// Default config setup
		Config();
	};

	// Renderer the Application is using
	enum class Renderer
	{
		None = -1,
		OpenGL,
		D3D11,
		Metal,
		Count
	};

	// Features available on the current Renderer
	struct RendererFeatures
	{
		// Whether Mesh Instancing is available
		bool instancing = false;

		// Whether the Texture origin is the bottom left.
		// This is true for OpenGL.
		bool origin_bottom_left = false;

		// Maximum Texture Size available
		int max_texture_size = 0;
	};

	class FrameBuffer;
	using FrameBufferRef = std::shared_ptr<FrameBuffer>;

	// Application
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