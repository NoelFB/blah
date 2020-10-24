#include <blah/streams/filestream.h>
#include <blah/internal/platform.h>
#include <blah/log.h>
#include <string.h>

using namespace Blah;

FileStream::FileStream()
{
	m_handle = nullptr;
	m_mode = FileMode::None;
}

FileStream::FileStream(const char* path, FileMode mode)
	: m_mode(mode)
{
	if (!Internal::Platform::file_open(path, &m_handle, mode))
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
		Internal::Platform::file_close(m_handle);
}

int64_t FileStream::length() const
{
	if (m_handle == nullptr)
		return 0;

	return Internal::Platform::file_length(m_handle);
}

int64_t FileStream::position() const
{
	if (m_handle == nullptr)
		return 0;

	return Internal::Platform::file_position(m_handle);
}

int64_t FileStream::seek(int64_t seek_to)
{
	if (m_handle == nullptr)
		return 0;

	return Internal::Platform::file_seek(m_handle, seek_to);
}

int64_t FileStream::read_into(void* ptr, int64_t length)
{
	if (m_handle == nullptr)
	{
		BLAH_ERROR("Unable to read from Stream");
		return 0;
	}

	return Internal::Platform::file_read(m_handle, ptr, length);
}

int64_t FileStream::write_from(const void* ptr, int64_t length)
{
	if (length <= 0)
		return 0;

	if (m_handle == nullptr)
	{
		BLAH_ERROR("Unable to write to Stream");
		return 0;
	}

	return Internal::Platform::file_write(m_handle, ptr, length);
}

void FileStream::close()
{
	if (m_handle != nullptr)
		Internal::Platform::file_close(m_handle);
	m_handle = nullptr;
	m_mode = FileMode::None;
}