#include <blah/streams/filestream.h>
#include <blah/common.h>
#include "../internal/platform.h"
#include <cstring>

using namespace Blah;

FileStream::FileStream(const FilePath& path, FileMode mode)
	: m_mode(mode)
	, m_file(File::open(path, mode))
{

}

FileStream::FileStream(FileStream&& src) noexcept
{
	m_file = src.m_file;
	m_mode = src.m_mode;
}

FileStream& FileStream::operator=(FileStream&& src) noexcept
{
	m_file = src.m_file;
	m_mode = src.m_mode;
	return *this;
}

size_t FileStream::length() const
{
	if (m_file)
		return m_file->length();
	return 0;
}

size_t FileStream::position() const
{
	if (m_file)
		return m_file->position();
	return 0;
}

size_t FileStream::seek(size_t seek_to)
{
	if (m_file)
		return m_file->seek(seek_to);
	return 0;
}

size_t FileStream::read_data(void* ptr, size_t length)
{
	if (length <= 0)
		return 0;

	if (m_file)
		return m_file->read((unsigned char*)ptr, length);
	return 0;
}

size_t FileStream::write_data(const void* ptr, size_t length)
{
	if (length <= 0)
		return 0;

	if (m_file)
		return m_file->write((const unsigned char*)ptr, length);
	return 0;
}

bool FileStream::is_open() const
{
	return m_file.get();
}

bool FileStream::is_readable() const
{
	return m_file.get() && (m_mode != FileMode::CreateWrite);
}

bool FileStream::is_writable() const
{
	return m_file.get() && (m_mode != FileMode::OpenRead);
}