#include <blah/streams/memorystream.h>
#include <cstring>

using namespace Blah;

MemoryStream::MemoryStream()
	: m_data(nullptr), m_length(0), m_position(0) {}

MemoryStream::MemoryStream(char* data, size_t length)
	: m_data(data), m_length(length), m_position(0) {}

MemoryStream::MemoryStream(MemoryStream&& src) noexcept
{
	m_data = src.m_data;
	m_position = src.m_position;
	m_length = src.m_length;
	src.m_data = nullptr;
	src.m_length = src.m_position = 0;
}

MemoryStream& MemoryStream::operator=(MemoryStream&& src) noexcept
{
	m_data = src.m_data;
	m_position = src.m_position;
	m_length = src.m_length;
	src.m_data = nullptr;
	src.m_length = src.m_position = 0;
	return *this;
}

size_t MemoryStream::length() const
{
	return m_length;
}

size_t MemoryStream::position() const
{
	return m_position;
}

size_t MemoryStream::seek(size_t seekTo)
{
	return m_position = (seekTo < 0 ? 0 : (seekTo > m_length ? m_length : seekTo));
}

size_t MemoryStream::read_data(void* ptr, size_t len)
{
	if (len < 0 || ptr == nullptr)
		return 0;

	if (len > m_length - m_position)
		len = m_length - m_position;

	memcpy(ptr, m_data + m_position, (size_t)len);
	m_position += len;
	return len;
}

size_t MemoryStream::write_data(const void* ptr, size_t len)
{
	if (len < 0 || ptr == nullptr)
		return 0;

	if (len > m_length - m_position)
		len = m_length - m_position;

	memcpy(m_data + m_position, ptr, (size_t)len);
	m_position += len;
	return len;
}

bool MemoryStream::is_open() const
{
	return m_data != nullptr;
}

bool MemoryStream::is_readable() const
{
	return true;
}

bool MemoryStream::is_writable() const
{
	return true;
}

void MemoryStream::close()
{
	m_data = nullptr; m_length = m_position = 0;
}

char* MemoryStream::data()
{
	return m_data;
}

const char* MemoryStream::data() const
{
	return m_data;
}
