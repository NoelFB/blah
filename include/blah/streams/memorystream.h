#pragma once
#include <blah/streams/stream.h>

namespace Blah
{
	class MemoryStream : public Stream
	{
	public:
		MemoryStream();
		MemoryStream(char* data, i64 length);
		MemoryStream(MemoryStream&& ms) noexcept;
		MemoryStream& operator=(MemoryStream&& ms) noexcept;
		~MemoryStream() { m_data = nullptr; m_length = m_position = 0; }

		virtual i64 length() const override { return m_length; }
		virtual i64 position() const override { return m_position; }
		virtual i64 seek(i64 seekTo) override { return m_position = (seekTo < 0 ? 0 : (seekTo > m_length ? m_length : seekTo)); }
		virtual bool is_open() const override { return m_data != nullptr; }
		virtual bool is_readable() const override { return true; }
		virtual bool is_writable() const override { return true; }
		virtual void close() override { m_data = nullptr; m_length = m_position = 0; }

		char* data() { return m_data; }
		const char* data() const { return m_data; }

	protected:
		virtual i64 read_into(void* ptr, i64 length) override;
		virtual i64 write_from(const void* ptr, i64 length) override;

	private:
		char* m_data;
		i64 m_length;
		i64 m_position;
	};
}