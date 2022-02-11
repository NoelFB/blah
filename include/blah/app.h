#pragma once
#include <blah/common.h>
#include <blah/numerics/spatial.h>

namespace Blah
{
	// Application Event Functions
	using AppEventFn = Func<void>;

	// Application Logging Functions
	using AppLogFn = Func<void, const char*, Log::Category>;

	// Type of Renderer the Application is using
	enum class RendererType
	{
		None = -1,
		OpenGL,
		D3D11,
	};

	// Renderer Information
	struct RendererFeatures
	{
		// The type of Renderer being used
		RendererType type = RendererType::None;

		// Whether Mesh Instancing is available
		bool instancing = false;

		// Whether the Texture origin is the bottom left.
		// This is true for OpenGL.
		bool origin_bottom_left = false;

		// Maximum Texture Size available
		int max_texture_size = 0;
	};

	// Application Configuration
	struct Config
	{
		// Application name.
		const char* name = "blah";
		
		// Which renderer to use
		// Default depends on the Platform
		RendererType renderer_type = RendererType::None;

		// Starting width, in pixels.
		// Depending on the OS DPI, the true window size may be a multiple of this.
		int width = 1280;

		// Starting height, in pixels.
		// Depending on the OS DPI, the true window size may be a multiple of this.
		int height = 720;

		// maximum updates to run before "giving up" and reducing frame rate.
		// this avoids the 'spiral of death'.
		// defaults to 5.
		int max_updates = 5;

		// target framerate.
		// defaults to 60.
		int target_framerate = 60;

		// Callback on application startup
		AppEventFn on_startup = nullptr;

		// Callback on application shutdown
		AppEventFn on_shutdown = nullptr;

		// Callback on application update
		AppEventFn on_update = nullptr;

		// Callback on application render
		AppEventFn on_render = nullptr;

		// Callback when the user has requested the application close.
		// For example, pressing the Close button, ALT+F4, etc
		// By default this calls `App::exit()`
		AppEventFn on_exit_request = nullptr;

		// Callback when the application logs info/warning/errors
		// Defaults to printf.
		AppLogFn on_log = nullptr;
	};

	// Forward declare Target for the BackBuffer
	class Target;
	using TargetRef = Ref<Target>;

	// Application
	namespace App
	{
		// Runs the application
		bool run(const Config* config);

		// Exits the application.
		// This only signals for the application to close, it will not stop
		// until the current update and render calls are finished.
		void exit();

		// Gets the config data used to run the application
		const Config& config();

		// Gets the working path
		const char* path();

		// Gets the user path
		const char* user_path();

		// Gets the Window Title
		const char* get_title();

		// Sets the Window Title
		void set_title(const char* title);

		// Gets the Window Position in Screen Coordinates
		Point get_position();

		// Sets the Window Position in Screen Coordinates
		void set_position(Point point);

		// Gets the size of the Window in Screen Coordinates.
		// On High DPI displays this may not be 1:1 with pixels.
		// For the size in pixels, use App::get_backbuffer_size()
		Point get_size();

		// Sets the Window Size in Screen Coordinates
		void set_size(Point point);

		// Gets the size of the BackBuffer, in pixels
		Point get_backbuffer_size();

		// Gets the content scale based on the platform.
		// macOS is usually 2.0, other platforms vary.
		float content_scale();

		// If the window is currently focused or has mouse input
		bool focused();

		// Toggles fullscreen if supported on the platform.
		// Otherwise this function does nothing.
		void fullscreen(bool enabled);

		// Retrieves the Renderer Features
		const RendererFeatures& renderer();

		// Gets the BackBuffer
		const TargetRef& backbuffer();
	}

	namespace System
	{
		// Tries to open the given URL in a web browser
		void open_url(const char* url);
	}
}