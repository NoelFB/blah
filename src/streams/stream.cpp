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