#include <blah/streams/filestream.h>
#include <blah/core/common.h>
#include "../internal/platform_backend.h"
#include <string.h>

using namespace Blah;

FileStream::FileStream()
{
	m_handle = nullptr;
	m_mode = FileMode::None;
}

FileStream::FileStream(const FilePath& path, FileMode mode)
	: m_mode(mode)
{
	if (!PlatformBackend::file_open(path, &m_handle, mode))
		m_handle = nullptr;
}

FileStream::FileStream(FileStream&& src) noexcept
{
	m_handle = src.m_handle;
	m_mode = src.m_mode;
	src.m_handle = nullptr;
}

FileStream& FileStream::operator=(FileStream&& src) noexcept
{
	m_handle = src.m_handle;
	m_mode = src.m_mode;
	src.m_handle = nullptr;
	return *this;
}

FileStream::~FileStream()
{
	if (m_handle != nullptr)
		PlatformBackend::file_close(m_handle);
}

i64 FileStream::length() const
{
	if (m_handle == nullptr)
		return 0;

	return PlatformBackend::file_length(m_handle);
}

i64 FileStream::position() const
{
	if (m_handle == nullptr)
		return 0;

	return PlatformBackend::file_position(m_handle);
}

i64 FileStream::seek(i64 seek_to)
{
	if (m_handle == nullptr)
		return 0;

	return PlatformBackend::file_seek(m_handle, seek_to);
}

bool FileStream::is_open() const
{
	return m_handle != nullptr;
}

bool FileStream::is_readable() const
{
	return m_handle != nullptr && (m_mode == FileMode::ReadWrite || m_mode == FileMode::Read);
}

bool FileStream::is_writable() const
{
	return m_handle != nullptr && (m_mode == FileMode::ReadWrite || m_mode == FileMode::Write);
}

i64 FileStream::read_into(void* ptr, i64 length)
{
	if (m_handle == nullptr)
	{
		BLAH_ERROR("Unable to read from Stream");
		return 0;
	}

	return PlatformBackend::file_read(m_handle, ptr, length);
}

i64 FileStream::write_from(const void* ptr, i64 length)
{
	if (length <= 0)
		return 0;

	if (m_handle == nullptr)
	{
		BLAH_ERROR("Unable to write to Stream");
		return 0;
	}

	return PlatformBackend::file_write(m_handle, ptr, length);
}

void FileStream::close()
{
	if (m_handle != nullptr)
		PlatformBackend::file_close(m_handle);
	m_handle = nullptr;
	m_mode = FileMode::None;
}