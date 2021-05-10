#pragma once
#include <blah/common.h>
#include <blah/numerics/point.h>

namespace Blah
{
	// Application Event Functions
	using AppEventFn = Func<void()>;

	// Application Logging Functions
	using AppLogFn = Func<void(const char* message, Log::Category category)>;

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

		// Gets the Window Position
		Point get_position();

		// Sets the Window Position
		void set_position(Point point);

		// Gets the Window Size
		Point get_size();

		// Sets the Window Size
		void set_size(Point point);

		// Gets the width of the window
		int width();

		// Gets the height of the window
		int height();
		
		// Gets the drawable width of the window, in pixels.
		// This may differ from the width when on platforms with High DPI Displays.
		int draw_width();

		// Gets the drawable height of the window, in pixels.
		// This may differ from the height when on platforms with High DPI Displays.
		int draw_height();

		// Gets the content scale based on the platform.
		// macOS is usually 2.0, other platforms vary.
		float content_scale();

		// Toggles fullscreen if supported on the platform.
		// Otherwise this function does nothing.
		void fullscreen(bool enabled);

		// Returns the Rendering API in use
		Renderer renderer();

		// Retrieves the Renderer Features
		const RendererFeatures& renderer_features();

		// Reference to the window's back buffer
		extern const TargetRef backbuffer;
	}
}