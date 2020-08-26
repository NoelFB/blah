#include <blah/streams/stream.h>
#include <blah/containers/str.h>
#include <blah/internal/platform.h>
#include <string.h>

using namespace Blah;

Stream::Stream()
{

}

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

String Stream::read_line()
{
	String string;
	read_line(string);
	return string;
}

int64_t Stream::read_line(String& writeTo)
{
	const int bufferSize = 512;
	char buffer[bufferSize];

	int64_t pos = position();
	int64_t length = 0;
	int64_t count = 0;
	bool hit = false;

	// read chunk-by-chunk
	do
	{
		count = (int)read(buffer, bufferSize);
		pos += count;

		// check for a newline
		int64_t end = count;
		for (int n = 0; n < count; n++)
			if (buffer[n] == '\n' || buffer[n] == '\r')
			{
				hit = true;
				end = n;

				// skip to the end of the line for future reading
				int64_t lineEnd = pos - count + end + 1;

				// there might be a trailing '\n'
				if (buffer[n] == '\r')
				{
					if (end < count && buffer[n + 1] == '\n')
					{
						lineEnd++;
					}
					// our buffer aligned perfectly ..... :/
					else if (count == bufferSize && end == count)
					{
						char ch;
						if (read(&ch, 1) != 0 && ch == '\n')
							lineEnd++;
					}
				}

				seek(lineEnd);
				break;
			}

		// copy to string
		writeTo.set_length((int)(length + end));
		memcpy(writeTo.cstr() + length, buffer, (size_t)end);
		*(writeTo.cstr() + length + end) = '\0';

		// increment length
		length += end;

	} while (!hit && count >= bufferSize);

	return length;
}

String Stream::read_string(int length)
{
	if (length >= 0)
	{
		String str;
		str.set_length(length);
		read_into(str.cstr(), length);
		str[length] = '\0';
		return str;
	}
	else
	{
		String str;
		char next;
		while (read_into(&next, 1) && next != '\0')
			str.append(next);
		return str;
	}
}