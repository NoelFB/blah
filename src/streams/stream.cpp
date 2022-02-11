#include <blah/streams/stream.h>
#include <blah/containers/str.h>
#include <string.h>

using namespace Blah;

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

// reads a string. if length < 0, assumes null-terminated
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
		result.set_length(length);
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

