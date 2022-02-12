#include <blah/streams/memorystream.h>
#include <cstring>

using namespace Blah;

MemoryStream::MemoryStream(u8* data, size_t length)
	: m_data(data), m_const_data(nullptr), m_length(length), m_position(0) {}

MemoryStream::MemoryStream(const u8* data, size_t length)
	: m_data(nullptr), m_const_data(data), m_length(length), m_position(0) {}

size_t MemoryStream::length() const
{
	return m_length;
}

size_t MemoryStream::position() const
{
	return m_position;
}

size_t MemoryStream::seek(size_t seek_to)
{
	return m_position = (seek_to < 0 ? 0 : (seek_to > m_length ? m_length : seek_to));
}

size_t MemoryStream::read_data(void* ptr, size_t len)
{
	const u8* src = (m_data ? m_data : m_const_data);

	if (src == nullptr || ptr == nullptr || len <= 0 || m_length <= 0 || m_position >= m_length)
		return 0;

	if (len > m_length - m_position)
		len = m_length - m_position;

	memcpy(ptr, src + m_position, len);
	m_position += len;
	return len;
}

size_t MemoryStream::write_data(const void* ptr, size_t len)
{
	if (m_data == nullptr || ptr == nullptr || len <= 0 || m_length <= 0 || m_position >= m_length)
		return 0;

	if (len > m_length - m_position)
		len = m_length - m_position;

	memcpy(m_data + m_position, ptr, len);
	m_position += len;
	return len;
}

bool MemoryStream::is_open() const
{
	return (m_data || m_const_data) && m_length > 0;
}

bool MemoryStream::is_readable() const
{
	return (m_data || m_const_data) && m_length > 0;
}

bool MemoryStream::is_writable() const
{
	return m_data != nullptr && m_length > 0;
}

u8* MemoryStream::data()
{
	return m_data;
}

const u8* MemoryStream::data() const
{
	return (m_data ? m_data : m_const_data);
}
