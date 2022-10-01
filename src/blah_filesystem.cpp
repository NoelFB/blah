#include <blah_filesystem.h>
#include "internal/blah_internal.h"

using namespace Blah;

FileRef File::open(const FilePath& path, FileMode mode)
{
	BLAH_ASSERT_PLATFORM();

	FileRef ref;
	if (App::Internal::platform)
		ref = App::Internal::platform->file_open(path.cstr(), mode);
	if (ref)
		ref->m_mode = mode;
	return ref;
}

bool File::exists(const FilePath& path)
{
	BLAH_ASSERT_PLATFORM();
	if (App::Internal::platform)
		return App::Internal::platform->file_exists(path.cstr());
	return false;
}

bool File::destroy(const FilePath& path)
{
	BLAH_ASSERT_PLATFORM();
	if (App::Internal::platform)
		return App::Internal::platform->file_delete(path.cstr());
	return false;
}

FileMode File::mode() const
{
	return m_mode;
}

bool Directory::create(const FilePath& path)
{
	BLAH_ASSERT_PLATFORM();
	if (App::Internal::platform)
		return App::Internal::platform->dir_create(path.cstr());
	return false;
}

bool Directory::exists(const FilePath& path)
{
	BLAH_ASSERT_PLATFORM();
	if (App::Internal::platform)
		return App::Internal::platform->dir_exists(path.cstr());
	return false;
}

bool Directory::destroy(const FilePath& path)
{
	BLAH_ASSERT_PLATFORM();
	if (App::Internal::platform)
		return App::Internal::platform->dir_delete(path.cstr());
	return false;
}

Vector<FilePath> Directory::enumerate(const FilePath& path, bool recursive)
{
	BLAH_ASSERT_PLATFORM();

	Vector<FilePath> list;

	if (App::Internal::platform)
	{
		App::Internal::platform->dir_enumerate(list, path.cstr(), recursive);
		for (auto& it : list)
		{
			for (int n = 0; n < it.length(); n ++)
				if (it[n] == '\\') it[n] = '/';
		}
	}

	return list;
}

void Directory::explore(const FilePath& path)
{
	BLAH_ASSERT_PLATFORM();
	if (App::Internal::platform)
		App::Internal::platform->dir_explore(path);
}

FilePath Path::get_file_name(const FilePath& path)
{
	const char* cstr = path.cstr();
	auto length = path.length();

	for (auto n = length; n > 0; n--)
		if (*(cstr + n - 1) == '/')
			return FilePath(cstr + n);

	return path;
}

FilePath Path::get_file_name_no_ext(const FilePath& path)
{
	FilePath filename = get_file_name(path);
	return get_path_no_ext(filename);
}

FilePath Path::get_path_no_ext(const FilePath& path)
{
	const char* cstr = path.cstr();
	for (int n = 0; n < path.length(); n++)
		if (*(cstr + n) == '.')
			return FilePath(cstr, cstr + n);
	return path;
}

FilePath Path::get_directory_name(const FilePath& path)
{
	FilePath directory = path;
	while (directory.ends_with("/"))
		directory = FilePath(directory.begin(), directory.end() - 1);
	auto last = directory.last_index_of('/');
	if (last >= 0)
		directory = FilePath(directory.begin(), directory.begin() + last + 1);
	return directory;
}

FilePath Path::get_path_after(const FilePath& path, const FilePath& after)
{
	for (int i = 0; i < path.length(); i++)
	{
		FilePath substr(path.begin() + i, path.end());
		if (substr.starts_with(after, false))
			return FilePath(path.begin() + i + after.length(), path.end());
	}

	return path;
}

FilePath Path::normalize(const FilePath& path)
{
	FilePath normalized;

	auto len = path.length();
	for (auto n = 0; n < len; n++)
	{
		// normalize slashes
		if (path[n] == '\\' || path[n] == '/')
		{
			if (normalized.length() == 0 || normalized[normalized.length() - 1] != '/')
				normalized.append('/');
		}
		// move up a directory
		else if (path[n] == '.' && n < len - 1 && path[n + 1] == '.')
		{
			// search backwards for last /
			bool could_move_up = false;
			if (normalized.length() > 0)
			{
				for (auto k = normalized.length() - 1; k > 0; k--)
					if (normalized[k - 1] == '/')
					{
						normalized = FilePath(normalized.begin(), normalized.begin() + k - 1);
						could_move_up = true;
						break;
					}
			}

			if (!could_move_up)
				normalized.append('.');
			else
				n++;
		}
		else
			normalized.append(path[n]);
	}

	return normalized;
}

FilePath Path::join(const FilePath& a, const FilePath& b)
{
	if (a.length() <= 0)
		return normalize(b);
	else if (b.length() <= 0)
		return normalize(a);
	else
		return normalize(a + "/" + b);
}