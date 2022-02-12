#include "blah/streams/bufferstream.h"
#include <string.h>

using namespace Blah;

BufferStream::BufferStream(int capacity)
{
	m_buffer.resize(capacity);
}

size_t BufferStream::length() const
{
	return m_buffer.size();
}

size_t BufferStream::position() const
{
	return m_position;
}

size_t BufferStream::seek(size_t seek_to)
{
	return m_position = (seek_to < 0 ? 0 : (seek_to > m_buffer.size() ? m_buffer.size() : seek_to));
}

size_t BufferStream::read_data(void* ptr, size_t len)
{
	if (ptr == nullptr || len <= 0)
		return 0;

	if (len > m_buffer.size() - m_position)
		len = m_buffer.size() - m_position;

	memcpy(ptr, m_buffer.data() + m_position, (size_t)len);
	m_position += len;
	return len;
}

size_t BufferStream::write_data(const void* ptr, size_t len)
{
	if (len < 0)
		return 0;

	// resize
	if (m_position + len > m_buffer.size())
		resize(m_position + len);

	// copy data
	if (ptr != nullptr)
		memcpy(m_buffer.data() + m_position, ptr, (size_t)len);

	// increment position
	m_position += len;

	// return the amount we wrote
	return len;
}

bool BufferStream::is_open() const { return true; }
bool BufferStream::is_readable() const { return true; }
bool BufferStream::is_writable() const { return true; }

void BufferStream::resize(size_t length)
{
	m_buffer.resize(length);
}

void BufferStream::clear()
{
	m_buffer.clear();
	m_position = 0;
}

u8* BufferStream::data()
{
	return m_buffer.data();
}

const u8* BufferStream::data() const
{
	return m_buffer.data();
}
