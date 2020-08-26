#pragma once

namespace Blah
{
	enum class GfxAPI
	{
		Any = -1,
		OpenGL,
		Count
	};

	struct Config
	{
		const char* name;
		int width;
		int height;
		int max_updates;
		int target_framerate;

		GfxAPI graphics;

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
	}
}