#pragma once
#include <blah/containers/str.h>
#include <blah/containers/vector.h>

namespace Blah
{
	using FilePath = StrOf<265>;
	
	enum class FileMode
	{
		None		= 0,
		Read		= 1 << 1,
		Write		= 1 << 2,
		ReadWrite	= Read | Write,
	};

	namespace Directory
	{
		bool create(const FilePath& path);
		bool exists(const FilePath& path);
		bool remove(const FilePath& path);
		Vector<FilePath> enumerate(const FilePath& str, bool recursive = true);
		void explore(const FilePath& path);
	}

	namespace File
	{
		bool exists(const FilePath& path);
		bool remove(const FilePath& path);
	}

	namespace Path
	{
		FilePath get_file_name(const FilePath& path);
		FilePath get_file_name_no_ext(const FilePath& path);
		FilePath get_path_no_ext(const FilePath& path);
		FilePath get_path_after(const FilePath& path, const FilePath& after);
		FilePath get_directory_name(const FilePath& path);
		FilePath normalize(const FilePath& path);
	}
}