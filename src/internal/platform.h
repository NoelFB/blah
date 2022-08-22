#pragma once
#include <blah/common.h>
#include <blah/input.h>
#include <blah/filesystem.h>
#include <blah/containers/vector.h>

namespace Blah
{
	struct Config;

	class Platform
	{
	public:

		virtual ~Platform() = default;

		// Initialize the Graphics
		virtual bool init(const Config& config) = 0;

		// Called after the on_startup callback, but before the update loop begins
		virtual void ready() = 0;

		// Called during shutdown
		virtual void shutdown() = 0;

		// The time, in ticks (microseconds) since the Application was started
		virtual u64 ticks() = 0;

		// Called every frame
		virtual void update(InputState& state) = 0;

		// Sleeps the current thread
		virtual void sleep(int milliseconds) = 0;

		// Called to present the window contents
		virtual void present() = 0;

		// Called when the App sets flags
		virtual void set_app_flags(u32 flags) = 0;

		// Gets the Application Window Title in UTF-8
		virtual const char* get_title() = 0;

		// Sets the Application Window Title in UTF-8
		virtual void set_title(const char* title) = 0;

		// Gets the Application Window Position, in Screen Coordinates
		virtual void get_position(int* x, int* y) = 0;

		// Sets the Application Window Position, in Screen Coordinates
		virtual void set_position(int x, int y) = 0;
		
		// Gets whether the Window has focus
		virtual bool get_focused() = 0;

		// Gets the Application Window Size, in Screen Coordinates
		virtual void get_size(int* width, int* height) = 0;

		// Sets the Application Window Size, in Screen Coordinates
		virtual void set_size(int width, int height) = 0;

		// Gets the Application Window Drawing Size, in Pixels. This may differ from the Window Size on hi-dpi displays.
		virtual void get_draw_size(int* width, int* height) = 0;

		// Gets the Desktop Content Scale. Gui should be scaled by this value
		virtual float get_content_scale() = 0;

		// Returns the absoluate path to the directory that the application was started from
		virtual const char* app_path() = 0;

		// Returns the absolute path to the user directory where save data and settings should be stored
		virtual const char* user_path() = 0;

		// Opens a file and sets the handle, or returns an empty handle if it fails
		virtual FileRef file_open(const char* path, FileMode mode) = 0;

		// Returns true if a file with the given path exists
		virtual bool file_exists(const char* path) = 0;

		// Returns true if a file with the given path was deleted
		virtual bool file_delete(const char* path) = 0;

		// Returns true if a directory with the given path was successfully created
		virtual bool dir_create(const char* path) = 0;

		// Returns true if a directory with the given path exists
		virtual bool dir_exists(const char* path) = 0;

		// Returns true if a directory with the given path was deleted
		virtual bool dir_delete(const char* path) = 0;

		// enumerates a directory and appends each file to the given list
		virtual void dir_enumerate(Vector<FilePath>& list, const char* path, bool recursive) = 0;

		// opens a directory in the OS file explorer / finder
		virtual void dir_explore(const char* path) = 0;

		// sets the contents of the clipboard
		virtual void set_clipboard(const char* text) = 0;

		// gets the contents of the clipboard into the given string
		virtual const char* get_clipboard() = 0;

		// Tries to open a URL in a web browser
		virtual void open_url(const char* url) = 0;

		// OpenGL Methods
		virtual void* gl_get_func(const char* name) = 0;
		virtual void* gl_context_create() = 0;
		virtual void gl_context_make_current(void* context) = 0;
		virtual void gl_context_destroy(void* context) = 0;

		// D3D11 Methods
		virtual void* d3d11_get_hwnd() = 0;

		// Instantiates the Platform object
		static Platform* try_make_platform(const Config& config);
	};
}