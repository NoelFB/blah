#pragma once
#include <blah/common.h>
#include <blah/containers/str.h>
#include <blah/streams/endian.h>
#include <string.h>

namespace Blah
{
	class Stream
	{
	public:
		Stream() = default;
		Stream(const Stream&) = delete;
		Stream& operator=(const Stream&) = delete;

		virtual ~Stream() = default;

		// returns the length of the stream
		virtual size_t length() const = 0;

		// returns the position of the stream
		virtual size_t position() const = 0;

		// seeks the position of the stream
		virtual size_t seek(size_t position) = 0;

		// returns true of the stream is open
		virtual bool is_open() const = 0;

		// returns true of the stream is readable
		virtual bool is_readable() const = 0;

		// returns true of the stream is writable
		virtual bool is_writable() const = 0;

		// closes the stream
		virtual void close() = 0;

		// pipes the contents of this stream to another stream
		size_t pipe(Stream& to, size_t length);

		// reads the amount of bytes into the given buffer, and returns the amount read
		size_t read(void* buffer, size_t length);

		// reads a string. if length < 0, assumes null-terminated
		String read_string(int length = -1);

		// reads a string until a newline '\n' or null-terminator '\0' is found
		String read_line();

		// reads a number
		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		T read()
		{
			return read<T>(Endian::Little);
		}

		// reads a number
		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		T read(Endian endian)
		{
			T result;
			read(&result, sizeof(T));
			if (!Blah::is_endian(endian))
				Blah::swap_endian(&result);
			return result;
		}

		// writes the amount of bytes to the stream from the given buffer, and returns the amount written
		size_t write(const void* buffer, size_t length);

		// writes the contents of a string to the stream
		size_t write(const String& string);

		// writes a number
		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		size_t write(const T& value)
		{
			return write<T>(value, Endian::Little);
		}

		// writes a number
		template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		size_t write(const T& value, Endian endian)
		{
			T writing = value;

			if (!Blah::is_endian(endian))
				Blah::swap_endian(&writing);

			return write(&writing, sizeof(T));
		}

	protected:
		// reads the amount of bytes into the given buffer, and returns the amount read
		virtual size_t read_data(void* buffer, size_t length) = 0;

		// writes the amount of bytes to the stream from the given buffer, and returns the amount written
		virtual size_t write_data(const void* buffer, size_t length) = 0;
	};
}