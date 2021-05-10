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
