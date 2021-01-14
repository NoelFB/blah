#include <blah/streams/stream.h>
#include <blah/containers/str.h>
#include <string.h>

using namespace Blah;

int64_t Stream::pipe(Stream& stream, int64_t length)
{
	const int BUFFER_LENGTH = 4096;
	int64_t result = 0;

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
		read_into(result.cstr(), length);
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

int64_t Stream::write(const void* buffer, int64_t length) 
{ 
	return write_from(buffer, length);
}

int64_t Stream::write(const String& string)
{
	return write_from(string.begin(), string.length());
}
