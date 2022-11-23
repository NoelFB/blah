#pragma once
#include <blah_common.h>
#include <blah_input.h>
#include <blah_filesystem.h>
#include <blah_vector.h>

namespace Blah
{
	struct Config;

	namespace Platform
	{
		// Initialize the Graphics
		bool init(const Config& config);

		// Called after the on_startup callback, but before the update loop begins
		void ready();

		// Called during shutdown
		void shutdown();

		// The time, in ticks (microseconds) since the Application was started
		u64 ticks();

		// Called every frame
		void update(InputState& state);

		// Sleeps the current thread
		void sleep(int milliseconds);

		// Called to present the window contents
		void present();

		// Called when the App sets flags
		void set_app_flags(u32 flags);

		// Gets the Application Window Title in UTF-8
		const char* get_title();

		// Sets the Application Window Title in UTF-8
		void set_title(const char* title);

		// Gets the Application Window Position, in Screen Coordinates
		void get_position(int* x, int* y);

		// Sets the Application Window Position, in Screen Coordinates
		void set_position(int x, int y);
		
		// Gets whether the Window has focus
		bool get_focused();

		// Gets the Application Window Size, in Screen Coordinates
		void get_size(int* width, int* height);

		// Sets the Application Window Size, in Screen Coordinates
		void set_size(int width, int height);

		// Gets the Application Window Drawing Size, in Pixels. This may differ from the Window Size on hi-dpi displays.
		void get_draw_size(int* width, int* height);

		// Returns the absolute path to the directory that the application was started from
		const char* app_path();

		// Returns the absolute path to the user directory where save data and settings should be stored
		const char* user_path();

		// Opens a file and sets the handle, or returns an empty handle if it fails
		FileRef file_open(const char* path, FileMode mode);

		// Returns true if a file with the given path exists
		bool file_exists(const char* path);

		// Returns true if a file with the given path was deleted
		bool file_delete(const char* path);

		// Returns true if a directory with the given path was successfully created
		bool dir_create(const char* path);

		// Returns true if a directory with the given path exists
		bool dir_exists(const char* path);

		// Returns true if a directory with the given path was deleted
		bool dir_delete(const char* path);

		// enumerates a directory and appends each file to the given list
		void dir_enumerate(Vector<FilePath>& list, const char* path, bool recursive);

		// opens a directory in the OS file explorer / finder
		void dir_explore(const char* path);

		// sets the contents of the clipboard
		void set_clipboard(const char* text);

		// gets the contents of the clipboard into the given string
		const char* get_clipboard();

		// Tries to open a URL in a web browser
		void open_url(const char* url);

		// OpenGL Methods
		void* gl_get_func(const char* name);
		void* gl_context_create();
		void gl_context_make_current(void* context);
		void gl_context_destroy(void* context);

		// D3D11 Methods
		void* d3d11_get_hwnd();
	};
}