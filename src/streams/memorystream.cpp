#include <blah/streams/memorystream.h>
#include <string.h>

using namespace Blah;

MemoryStream::MemoryStream()
	: m_data(nullptr), m_length(0), m_position(0) {}

MemoryStream::MemoryStream(char* data, i64 length)
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

i64 MemoryStream::read_into(void* ptr, i64 len)
{
	if (len < 0 || ptr == nullptr)
		return 0;

	if (len > m_length - m_position)
		len = m_length - m_position;

	memcpy(ptr, m_data + m_position, (size_t)len);
	m_position += len;
	return len;
}

i64 MemoryStream::write_from(const void* ptr, i64 len)
{
	if (len < 0 || ptr == nullptr)
		return 0;

	if (len > m_length - m_position)
		len = m_length - m_position;

	memcpy(m_data + m_position, ptr, (size_t)len);
	m_position += len;
	return len;
}
