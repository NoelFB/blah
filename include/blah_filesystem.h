#pragma once
#include <blah_common.h>
#include <blah_string.h>
#include <blah_vector.h>

namespace Blah
{
	class File;
	using FileRef = Ref<File>;

	enum class FileMode
	{
		// Opens an existing file for reading.
		OpenRead,

		// Opens an existing file for reading and writing.
		Open,

		// Creates a new file or overwrites an existing file for writing.
		CreateWrite,

		// Creates a new file or overwrites an existing file for reading and writing.
		Create,
	};

	class File
	{
	protected:
		File() = default;

	public:
		// Opens a file at the given path.
		// If it fails, this will return an empty reference.
		static FileRef open(const FilePath& path, FileMode mode);

		// checks if the given file exists
		static bool exists(const FilePath& path);

		// deletes the given file
		static bool destroy(const FilePath& path);

		// Default Destructor
		virtual ~File() = default;

		// Gets the Mode the File was opened with
		FileMode mode() const;

		// Gets the File Length
		virtual size_t length() = 0;

		// Gets the current File Position
		virtual size_t position() = 0;

		// Seeks to the given position in the File
		virtual size_t seek(size_t position) = 0;

		// Reads from the File into the buffer, and returns how many bytes were successfully read
		virtual size_t read(void* buffer, size_t length) = 0;

		// Writes from the buffer into the File, nd returns how many bytes were successfully written
		virtual size_t write(const void* buffer, size_t length) = 0;

	private:
		FileMode m_mode;
	};

	namespace Directory
	{
		// Creates a new directory at the given location.
		// Returns false if unable to create the directory.
		bool create(const FilePath& path);

		// Returns whether the given directory exists
		bool exists(const FilePath& path);

		// Tries to delete a path and returns whether it was successful
		bool destroy(const FilePath& path);

		// Enumerates over a directory and returns a list of files & directories
		Vector<FilePath> enumerate(const FilePath& path, bool recursive = true);

		// Opens the path in the File Explorer / Finder
		void explore(const FilePath& path);
	}

	namespace Path
	{
		// Returns the file name of the path
		FilePath get_file_name(const FilePath& path);

		// Returns the file name of the path, without the file extension
		FilePath get_file_name_no_ext(const FilePath& path);

		// Returns the path without any file extensions
		FilePath get_path_no_ext(const FilePath& path);

		// Returns relative path
		FilePath get_path_after(const FilePath& path, const FilePath& after);

		// Gets the top directory name from the path
		FilePath get_directory_name(const FilePath& path);

		// Normalizes a path (removes ../, changes \\ to /, removes redundant slashes, etc)
		FilePath normalize(const FilePath& path);

		// Joins two paths together
		FilePath join(const FilePath& a, const FilePath& b);

		// Joins two paths together
		template<typename ... Args>
		FilePath join(const FilePath& a, const FilePath& b, const Args&... args)
		{
			return join(a, join(b, args...));
		}
	}
}