#pragma once
#include <blah_common.h>
#include <blah_graphics.h>
#include <blah_spatial.h>

namespace Blah
{
	// Application Event Functions
	using AppEventFn = Func<void>;

	// Application Logging Functions
	using AppLogFn = Func<void, const char*, Log::Category>;

	// Application flags
	namespace Flags
	{
		constexpr u32 FixedTimestep = 1 << 0;
		constexpr u32 VSync = 1 << 1;
		constexpr u32 Fullscreen = 1 << 2;
		constexpr u32 Resizable = 1 << 3;
	}

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

		// target framerate when running with Fixed Timestep
		// defaults to 60.
		int target_framerate = 60;

		// The expected freqeuncy of audio files and how quickly to play audio.
		int audio_frequency_in_Hz = 44100;

		// default starting flags
		u32 flags = Flags::VSync | Flags::Resizable | Flags::FixedTimestep;

		// Callback on application startup
		AppEventFn on_startup = nullptr;

		// Callback on application shutdown
		AppEventFn on_shutdown = nullptr;

		// Callback on application update
		AppEventFn on_update = nullptr;

		// Callback on application render
		AppEventFn on_render = nullptr;

		// Callback when the user has requested the application close.
		// For example, pressing the Close button, ALT+F4, etc.
		// By default this calls `App::exit()`
		AppEventFn on_exit_request = nullptr;

		// Callback when the application logs info/warning/errors
		// Defaults to printf.
		AppLogFn on_log = nullptr;
	};

	// Application
	namespace App
	{
		// Runs the application
		bool run(const Config* config);

		// Checks if the Application is running yet
		bool is_running();

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

		// Toggles a specific flag
		void set_flag(u32 flag, bool enabled);

		// Gets whether a specific flag is enabled
		bool get_flag(u32 flag);

		// Toggles the fullscreen flag
		inline void set_fullscreen(bool enabled) { set_flag(Flags::Fullscreen, enabled); }

		// Toggles the V-Sync flag
		inline void set_vsync(bool enabled) { set_flag(Flags::VSync, enabled); }

		// Toggles the resizable flag
		inline void set_resizable(bool enabled) { set_flag(Flags::Resizable, enabled); }

		// Toggles whether to update with Fixed Timestep
		inline void set_fixedtimestep(bool enabled) { set_flag(Flags::FixedTimestep, enabled); }

		// Retrieves the Renderer Information
		const RendererInfo& renderer();

		// Gets the BackBuffer
		const TargetRef& backbuffer();
	}

	namespace System
	{
		// Tries to open the given URL in a web browser
		void open_url(const char* url);
	}
}