#pragma once
#include <inttypes.h>
#include <blah/filesystem.h>
#include <blah/containers/list.h>

namespace Blah
{
	struct Config;
	enum class FileMode;

	namespace Internal
	{
		namespace Platform
		{
			typedef void* FileHandle;

			// Initialize the System
			bool init(const Config* config);

			// Called after the on_startup callback, but before the update loop begins
			void ready();

			// Called during shutdown
			void shutdown();

			// The time, in milliseconds, since the Application was started
			uint64_t time();

			// Called every frame
			void frame();

			// Sleeps the current thread
			void sleep(int milliseconds);

			// Called to present the window contents
			void present();

			// Gets the Application Window Title in UTF-8
			const char* get_title();

			// Sets the Application Window Title in UTF-8
			void set_title(const char* title);

			// Gets the Application Window Position, in Screen Coordinates
			void get_position(int* x, int* y);

			// Sets the Application Window Position, in Screen Coordinates
			void set_position(int x, int y);

			// Sets the Window Fullscreen if enabled is not 0
			void set_fullscreen(bool enabled);

			// Gets the Application Window Size, in Screen Coordinates
			void get_size(int* width, int* height);

			// Sets the Application Window Size, in Screen Coordinates
			void set_size(int width, int height);

			// Gets the Application Window Drawing Size, in Pixels. This may differ from the Window Size on hi-dpi displays.
			void get_draw_size(int* width, int* height);

			// Gets the Desktop Content Scale. Gui should be scaled by this value
			float get_content_scale();

			// Returns the absoluate path to the directory that the application was started from
			const char* app_path();

			// Returns the absolute path to the user directory where save data and settings should be stored
			const char* user_path();

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
			void dir_enumerate(List<FilePath>& list, const char* path, bool recursive);

			// opens a directory in the OS file explorer / finder
			void dir_explore(const char* path);

			// Opens a file and sets the handle. returns true if the file was successfully opened
			bool file_open(const char* path, FileHandle* handle, FileMode mode);

			// Returns the length of the file
			int64_t file_length(FileHandle file);

			// Returns the Position of the file
			int64_t file_position(FileHandle file);

			// Seeks the Position of the file and returns the new position from the start of the file
			int64_t file_seek(FileHandle file, int64_t seekTo);

			// Reads a specific number of elements of a given size from the file into ptr
			int64_t file_read(FileHandle file, void* ptr, int64_t size);

			// Writes a specific number of elements of the given size from ptr to the file
			int64_t file_write(FileHandle file, const void* ptr, int64_t size);

			// Closes a file
			void file_close(FileHandle file);

			// OpenGL Methods
			void* gl_get_func(const char* name);
			void* gl_context_create();
			void gl_context_make_current(void* context);
			void gl_context_destroy(void* context);
		}
	}
}