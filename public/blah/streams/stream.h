#pragma once
#include <inttypes.h>
#include <blah/containers/str.h>

#define BLAH_SWAP_ENDIAN(value, type) \
	for (int i = 0; i < sizeof(type) / 2; i ++) { \
		uint8_t* _ptr = (uint8_t*)&value;\
		uint8_t _temp = *(_ptr + i); \
		*(_ptr + i) = *(_ptr + sizeof(type) - i - 1); \
		*(_ptr + sizeof(type) - i - 1) = _temp; \
	}

#define BLAH_BIG_ENDIAN (*((short*)"AB") == 0x4243)

namespace Blah
{
	enum class Endian
	{
		Little,
		Big
	};

	class Stream
	{
	public:
		Stream();
		Stream(const Stream&) = delete;
		Stream& operator=(const Stream&) = delete;

		virtual ~Stream() = default;

		// returns the length of the stream
		virtual int64_t length() const = 0;

		// returns the position of the stream
		virtual int64_t position() const = 0;

		// seeks the position of the stream
		virtual int64_t seek(int64_t seek_to) = 0;

		// returns true of the stream is open
		virtual bool is_open() const = 0;

		// returns true of the stream is readable
		virtual bool is_readable() const = 0;

		// returns true of the stream is writable
		virtual bool is_writable() const = 0;

		// closes the stream
		virtual void close() = 0;

		// pipes the contents of this tream to another stream
		int64_t pipe(Stream& to, int64_t length);

		// reads a single line from this stream (up until \r or \n)
		String read_line();

		// reada a single line from this stream, to the given string (up until \r or \n)
		int64_t read_line(String& writeTo);

		// reads a string of a given length, or until a null terminator if -1
		String read_string(int length = -1);

		// reads the amount of bytes into the given buffer, and returns the amount read
		int64_t read(void* buffer, int64_t length) { return read_into(buffer, length); }

		template<class T>
		T read() { return read<T>(Endian::Little); }

		template<class T>
		T read(Endian endian)
		{
			T value;
			read(&value, sizeof(T));
			
			if ((endian == Endian::Little && BLAH_BIG_ENDIAN) || (endian == Endian::Big && !BLAH_BIG_ENDIAN))
				BLAH_SWAP_ENDIAN(value, T);

			return value;
		}

		int64_t write(const void* buffer, int64_t length) { return write_from(buffer, length); }

		template<class T>
		int64_t write(const T& value) { return write<T>(value, Endian::Little); }

		template<class T>
		int64_t write(const T& value, Endian endian)
		{
			T writing = value;
			if ((endian == Endian::Little && BLAH_BIG_ENDIAN) || (endian == Endian::Big && !BLAH_BIG_ENDIAN))
				BLAH_SWAP_ENDIAN(writing, T);
			return write(&writing, sizeof(T));
		}

	protected:
		// reads from the stream into the given buffer, and returns the number of bytes read
		virtual int64_t read_into(void* buffer, int64_t length) = 0;

		// writes from the stream from the given buffer, and returns the number of bytes written
		virtual int64_t write_from(const void* buffer, int64_t length) = 0;

	};
}

#undef BLAH_SWAP_ENDIAN
#undef BLAH_BIG_ENDIAN