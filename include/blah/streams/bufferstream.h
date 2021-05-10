#pragma once
#include <blah/streams/stream.h>

namespace Blah
{
	class BufferStream : public Stream
	{
	public:
		BufferStream();
		BufferStream(int capacity);
		BufferStream(BufferStream&& bs) noexcept;
		BufferStream& operator=(BufferStream&& bs) noexcept;
		~BufferStream();

		virtual size_t length() const override { return m_length; }
		virtual size_t position() const override { return m_position; }
		virtual size_t seek(size_t seekTo) override { return m_position = (seekTo < 0 ? 0 : (seekTo > m_length ? m_length : seekTo)); }
		virtual bool is_open() const override { return m_buffer != nullptr; }
		virtual bool is_readable() const override { return true; }
		virtual bool is_writable() const override { return true; }
		virtual void close() override;

		void resize(size_t length);
		void clear() { m_length = m_position = 0; }

		char* data() { return m_buffer; }
		const char* data() const { return m_buffer; }

	protected:
		virtual size_t read_into(void* ptr, size_t length) override;
		virtual size_t write_from(const void* ptr, size_t length) override;

	private:
		char* m_buffer;
		size_t m_capacity;
		size_t m_length;
		size_t m_position;
	};
}