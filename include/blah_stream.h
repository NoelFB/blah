#pragma once
#include <blah_common.h>
#include <blah_string.h>
#include <blah_vector.h>
#include <blah_calc.h>
#include <blah_filesystem.h>

namespace Blah
{
	// Stream is a generic handler to read and write data to various buffers.
	// The purpose is to allow serializers to not care what they're writing data to,
	// be it a file or memory.
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

		// pipes the contents of this stream to another stream
		size_t pipe(Stream& to, size_t length);

		// reads the amount of bytes into the given buffer, and returns the amount read
		size_t read(void* buffer, size_t length);

		// reads a string. if length < 0, assumes null-terminated
		String read_string(int length = -1);

		// reads a string until a newline '\n' or null-terminator '\0' is found
		String read_line();

		// reads a number, returns the value read.
		u8  read_u8 (Endian endian = Endian::Little);
		u16 read_u16(Endian endian = Endian::Little);
		u32 read_u32(Endian endian = Endian::Little);
		u64 read_u64(Endian endian = Endian::Little);
		i8  read_i8 (Endian endian = Endian::Little);
		i16 read_i16(Endian endian = Endian::Little);
		i32 read_i32(Endian endian = Endian::Little);
		i64 read_i64(Endian endian = Endian::Little);
		f32 read_f32(Endian endian = Endian::Little);
		f64 read_f64(Endian endian = Endian::Little);

		// writes the amount of bytes to the stream from the given buffer, and returns the amount written
		size_t write(const void* buffer, size_t length);

		// writes the contents of a string to the stream
		size_t write(const String& string);

		// writes a number, returns number of bytes written
		size_t write_u8 (u8  value, Endian endian = Endian::Little);
		size_t write_u16(u16 value, Endian endian = Endian::Little);
		size_t write_u32(u32 value, Endian endian = Endian::Little);
		size_t write_u64(u64 value, Endian endian = Endian::Little);
		size_t write_i8 (i8  value, Endian endian = Endian::Little);
		size_t write_i16(i16 value, Endian endian = Endian::Little);
		size_t write_i32(i32 value, Endian endian = Endian::Little);
		size_t write_i64(i64 value, Endian endian = Endian::Little);
		size_t write_f32(f32 value, Endian endian = Endian::Little);
		size_t write_f64(f64 value, Endian endian = Endian::Little);

	protected:
		// reads the amount of bytes into the given buffer, and returns the amount read
		virtual size_t read_data(void* buffer, size_t length) = 0;

		// writes the amount of bytes to the stream from the given buffer, and returns the amount written
		virtual size_t write_data(const void* buffer, size_t length) = 0;
	};

	// File Stream reads & writes over a File handle
	class FileStream : public Stream
	{
	public:
		FileStream() = default;
		FileStream(const FilePath& path, FileMode mode);
		FileStream(const FileRef& file);

		size_t length() const override;
		size_t position() const override;
		size_t seek(size_t position) override;
		bool is_open() const override;
		bool is_readable() const override;
		bool is_writable() const override;

	protected:
		size_t read_data(void* ptr, size_t length) override;
		size_t write_data(const void* ptr, size_t length) override;

	private:
		FileRef m_file;
	};

	// Memory Stream moves over an existing buffer.
	// The Buffer must exist for the duration of the Memory Stream.
	class MemoryStream : public Stream
	{
	public:
		MemoryStream() = default;
		MemoryStream(u8* data, size_t length);
		MemoryStream(const u8* data, size_t length);

		size_t length() const override;
		size_t position() const override;
		size_t seek(size_t seek_to) override;
		bool is_open() const override;
		bool is_readable() const override;
		bool is_writable() const override;

		u8* data();
		const u8* data() const;

	protected:
		size_t read_data(void* ptr, size_t length) override;
		size_t write_data(const void* ptr, size_t length) override;

	private:
		u8* m_data = nullptr;
		const u8* m_const_data = nullptr;
		size_t m_length = 0;
		size_t m_position = 0;
	};

	// Buffer Stream reads and writes to an internal buffer.
	// It will grow the capacity if it needs to while writing.
	class BufferStream : public Stream
	{
	public:
		BufferStream() = default;
		BufferStream(int capacity);

		size_t length() const override;
		size_t position() const override;
		size_t seek(size_t seek_to) override;
		bool is_open() const override;
		bool is_readable() const override;
		bool is_writable() const override;

		void resize(size_t length);
		void clear();
		u8* data();
		const u8* data() const;

	protected:
		size_t read_data(void* ptr, size_t length) override;
		size_t write_data(const void* ptr, size_t length) override;

	private:
		Vector<u8> m_buffer;
		size_t m_position = 0;
	};
}