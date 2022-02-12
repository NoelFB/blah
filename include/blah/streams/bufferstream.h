#pragma once
#include <blah/streams/stream.h>

namespace Blah
{
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