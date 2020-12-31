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

		virtual int64_t length() const override { return m_length; }
		virtual int64_t position() const override { return m_position; }
		virtual int64_t seek(int64_t seekTo) override { return m_position = (seekTo < 0 ? 0 : (seekTo > m_length ? m_length : seekTo)); }
		virtual bool is_open() const override { return m_buffer != nullptr; }
		virtual bool is_readable() const override { return true; }
		virtual bool is_writable() const override { return true; }
		virtual void close() override;
		void clear() { m_length = m_position = 0; }

		char* data() { return m_buffer; }
		const char* data() const { return m_buffer; }

	protected:
		virtual int64_t read_into(void* ptr, int64_t length) override;
		virtual int64_t write_from(const void* ptr, int64_t length) override;

	private:
		char* m_buffer;
		int64_t m_capacity;
		int64_t m_length;
		int64_t m_position;
	};
}