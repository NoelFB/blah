#pragma once
#include <blah/containers/str.h>
#include <blah/containers/vector.h>

namespace Blah
{
	using FilePath = StrOf<265>;
	class FileStream;
	
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

	namespace Directory
	{
		// Creates a new directory at the given location.
		// Returns false if unable to create the directory.
		bool create(const FilePath& path);

		// Returns whether the given directory exists
		bool exists(const FilePath& path);

		// Tries to delete a path and returns whether it was successful
		bool remove(const FilePath& path);

		// Enumerates over a directory and returns a list of files & directories
		Vector<FilePath> enumerate(const FilePath& path, bool recursive = true);

		// Opens the path in the File Explorer / Finder
		void explore(const FilePath& path);
	}

	namespace File
	{
		// Checks if the given file exists
		bool exists(const FilePath& path);

		// Tries to delete a file and returns whether it was successful
		bool remove(const FilePath& path);

		// Opens the given file and returns a stream
		FileStream open(const FilePath& path, FileMode mode);
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
			return join(a, join(b, std::forward<Args>(args)...));
		}
	}
}