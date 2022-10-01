#include <blah_stream.h>
#include <blah_string.h>

using namespace Blah;

// Stream Base Class Implementation

size_t Stream::pipe(Stream& stream, size_t length)
{
	const int BUFFER_LENGTH = 4096;
	size_t result = 0;

	char buffer[BUFFER_LENGTH];
	while (length > 0)
	{
		auto step = length;
		if (step > BUFFER_LENGTH)
			step = BUFFER_LENGTH;

		auto count = read(buffer, step);
		auto wrote = stream.write(buffer, count);
		result += wrote;
		length -= step;

		if (count < step || wrote < count)
			break;
	}

	return result;
}

size_t Stream::read(void* buffer, size_t length)
{
	return read_data(buffer, length);
}

String Stream::read_string(int length)
{
	String result;

	if (length < 0)
	{
		char next;
		while (read(&next, 1) && next != '\0')
			result.append(next);
	}
	else
	{
		result.append('\0', length);
		read(result.cstr(), length);
	}

	return result;
}

String Stream::read_line()
{
	String result;

	char next;
	while (read(&next, 1) && next != '\n' && next != '\0')
		result.append(next);

	return result;
}

size_t Stream::write(const void* buffer, size_t length)
{
	return write_data(buffer, length);
}

size_t Stream::write(const String& string)
{
	return write(string.begin(), string.length());
}

u8 Stream::read_u8(Endian endian)
{
	u8 value; read_data(&value, sizeof(u8));
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return value;
}

u16 Stream::read_u16(Endian endian)
{
	u16 value; read_data(&value, sizeof(u16));
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return value;
}

u32 Stream::read_u32(Endian endian)
{
	u32 value; read_data(&value, sizeof(u32));
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return value;
}

u64 Stream::read_u64(Endian endian)
{
	u64 value; read_data(&value, sizeof(u64));
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return value;
}

i8 Stream::read_i8(Endian endian)
{
	i8 value; read_data(&value, sizeof(i8));
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return value;
}

i16 Stream::read_i16(Endian endian)
{
	i16 value; read_data(&value, sizeof(i16));
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return value;
}

i32 Stream::read_i32(Endian endian)
{
	i32 value; read_data(&value, sizeof(i32));
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return value;
}

i64 Stream::read_i64(Endian endian)
{
	i64 value; read_data(&value, sizeof(i64));
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return value;
}

float  Stream::read_f32(Endian endian)
{
	float value; read_data(&value, sizeof(float));
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return value;
}

double Stream::read_f64(Endian endian)
{
	double value; read_data(&value, sizeof(double));
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return value;
}

size_t Stream::write_u8(u8 value, Endian endian)
{
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return write_data(&value, sizeof(u8));
}

size_t Stream::write_u16(u16 value, Endian endian)
{
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return write_data(&value, sizeof(u16));
}

size_t Stream::write_u32(u32 value, Endian endian)
{
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return write_data(&value, sizeof(u32));
}

size_t Stream::write_u64(u64 value, Endian endian)
{
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return write_data(&value, sizeof(u64));
}

size_t Stream::write_i8(i8 value, Endian endian)
{
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return write_data(&value, sizeof(i8));
}

size_t Stream::write_i16(i16 value, Endian endian)
{
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return write_data(&value, sizeof(i16));
}

size_t Stream::write_i32(i32 value, Endian endian)
{
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return write_data(&value, sizeof(i32));
}

size_t Stream::write_i64(i64 value, Endian endian)
{
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return write_data(&value, sizeof(i64));
}

size_t Stream::write_f32(float value, Endian endian)
{
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return write_data(&value, sizeof(float));
}

size_t Stream::write_f64(double value, Endian endian)
{
	if (!Calc::is_endian(endian)) Calc::swap_endian(&value);
	return write_data(&value, sizeof(double));
}


// File Stream Implementation

FileStream::FileStream(const FilePath& path, FileMode mode)
	: m_file(File::open(path, mode)) {}

FileStream::FileStream(const FileRef& file)
	: m_file(file) {}

size_t FileStream::length() const
{
	return (m_file ? m_file->length() : 0);
}

size_t FileStream::position() const
{
	return (m_file ? m_file->position() : 0);
}

size_t FileStream::seek(size_t seek_to)
{
	return (m_file ? m_file->seek(seek_to) : 0);
}

size_t FileStream::read_data(void* ptr, size_t length)
{
	return (m_file ? m_file->read(ptr, length) : 0);
}

size_t FileStream::write_data(const void* ptr, size_t length)
{
	return (m_file ? m_file->write(ptr, length) : 0);
}

bool FileStream::is_open() const
{
	return m_file ? true : false;
}

bool FileStream::is_readable() const
{
	return m_file && m_file->mode() != FileMode::CreateWrite;
}

bool FileStream::is_writable() const
{
	return m_file && m_file->mode() != FileMode::OpenRead;
}


// Memory Stream Implementation

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

bool MemoryStream::is_open() const { return (m_data || m_const_data) && m_length > 0; }
bool MemoryStream::is_readable() const { return (m_data || m_const_data) && m_length > 0; }
bool MemoryStream::is_writable() const { return m_data != nullptr && m_length > 0; }
u8* MemoryStream::data() { return m_data; }
const u8* MemoryStream::data() const { return (m_data ? m_data : m_const_data); }


// Buffer Stream Implementation

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
