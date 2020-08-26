#include <blah/streams/bufferstream.h>
#include <string.h>

using namespace Blah;

BufferStream::BufferStream()
	: m_capacity(0), m_length(0), m_position(0) {}

BufferStream::BufferStream(int capacity)
	: m_capacity(0), m_length(0), m_position(0)
{
	if (capacity > 0)
	{
		m_buffer = new char[capacity];
		m_capacity = capacity;
	}
}

BufferStream::BufferStream(BufferStream&& src) noexcept
{
	m_buffer = src.m_buffer;
	m_length = src.m_length;
	m_capacity = src.m_capacity;
	m_position = src.m_position;
	src.m_buffer = nullptr;
	src.m_position = src.m_length = src.m_capacity = 0;
}

BufferStream& BufferStream::operator=(BufferStream&& src) noexcept
{
	m_buffer = src.m_buffer;
	m_length = src.m_length;
	m_capacity = src.m_capacity;
	m_position = src.m_position;
	src.m_buffer = nullptr;
	src.m_position = src.m_length = src.m_capacity = 0;
	return *this;
}

BufferStream::~BufferStream()
{
	delete[] m_buffer;
}

int64_t BufferStream::read_into(void* ptr, int64_t len)
{
	if (m_buffer == nullptr || ptr == nullptr)
		return 0;

	if (len < 0)
		return 0;

	if (len > m_length - m_position)
		len = m_length - m_position;

	memcpy(ptr, m_buffer + m_position, (size_t)len);
	m_position += len;
	return len;
}

int64_t BufferStream::write_from(const void* ptr, int64_t len)
{
	if (len < 0)
		return 0;

	// resize
	if (m_position + len >= m_capacity)
	{
		auto last_capacity = m_capacity;

		if (m_capacity <= 0)
			m_capacity = 16;
		while (m_position + len >= m_capacity)
			m_capacity *= 2;

		char* new_buffer = new char[m_capacity];
		
		if (m_buffer != nullptr)
		{
			memcpy(new_buffer, m_buffer, last_capacity);
			delete[] m_buffer;
		}

		m_buffer = new_buffer;
	}

	// copy data
	if (ptr != nullptr)
		memcpy(m_buffer + m_position, ptr, (size_t)len);

	// increment position
	m_position += len;
	if (m_position > m_length)
		m_length = m_position;

	// return the amount we wrote
	return len;
}

void BufferStream::close()
{
	delete[] m_buffer;
	m_buffer = nullptr;
	m_position = 0;
	m_length = 0;
	m_capacity = 0;
}