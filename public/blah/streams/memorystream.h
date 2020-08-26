#pragma once
#include <blah/streams/stream.h>

namespace Blah
{
	class MemoryStream : public Stream
	{
	public:
		MemoryStream();
		MemoryStream(char* data, int64_t length);
		MemoryStream(MemoryStream&& ms) noexcept;
		MemoryStream& operator=(MemoryStream&& ms) noexcept;
		~MemoryStream() { m_data = nullptr; m_length = m_position = 0; }

		virtual int64_t length() const override { return m_length; }
		virtual int64_t position() const override { return m_position; }
		virtual int64_t seek(int64_t seekTo) override { return m_position = (seekTo < 0 ? 0 : (seekTo > m_length ? m_length : seekTo)); }
		virtual bool is_open() const override { return m_data != nullptr; }
		virtual bool is_readable() const override { return true; }
		virtual bool is_writable() const override { return true; }
		virtual void close() override { m_data = nullptr; m_length = m_position = 0; }

		char* data() { return m_data; }
		const char* data() const { return m_data; }

	protected:
		virtual int64_t read_into(void* ptr, int64_t length) override;
		virtual int64_t write_from(const void* ptr, int64_t length) override;

	private:
		char* m_data;
		int64_t m_length;
		int64_t m_position;
	};
}