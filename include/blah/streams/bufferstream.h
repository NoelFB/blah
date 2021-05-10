#pragma once
#include <blah/streams/stream.h>

namespace Blah
{
	// Buffer Stream reads and writes to an internal buffer.
	// It will grow the capacity if it needs to while writing.
	class BufferStream : public Stream
	{
	public:
		BufferStream();
		BufferStream(int capacity);
		BufferStream(BufferStream&& bs) noexcept;
		BufferStream& operator=(BufferStream&& bs) noexcept;
		~BufferStream();

		size_t length() const override;
		size_t position() const override;
		size_t seek(size_t seekTo) override;
		bool is_open() const override;
		bool is_readable() const override;
		bool is_writable() const override;
		void close() override;

		void resize(size_t length);
		void clear();

		char* data();
		const char* data() const;

	protected:
		size_t read_data(void* ptr, size_t length) override;
		size_t write_data(const void* ptr, size_t length) override;

	private:
		char* m_buffer;
		size_t m_capacity;
		size_t m_length;
		size_t m_position;
	};
}