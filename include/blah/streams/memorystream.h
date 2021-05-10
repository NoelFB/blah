#pragma once
#include <blah/streams/stream.h>

namespace Blah
{
	class MemoryStream : public Stream
	{
	public:
		MemoryStream();
		MemoryStream(char* data, size_t length);
		MemoryStream(MemoryStream&& ms) noexcept;
		MemoryStream& operator=(MemoryStream&& ms) noexcept;
		~MemoryStream() override { m_data = nullptr; m_length = m_position = 0; }

		size_t length() const override { return m_length; }
		size_t position() const override { return m_position; }
		size_t seek(size_t seekTo) override { return m_position = (seekTo < 0 ? 0 : (seekTo > m_length ? m_length : seekTo)); }
		bool is_open() const override { return m_data != nullptr; }
		bool is_readable() const override { return true; }
		bool is_writable() const override { return true; }
		void close() override { m_data = nullptr; m_length = m_position = 0; }

		char* data() { return m_data; }
		const char* data() const { return m_data; }

	protected:
		size_t read_into(void* ptr, size_t length) override;
		size_t write_from(const void* ptr, size_t length) override;

	private:
		char* m_data;
		size_t m_length;
		size_t m_position;
	};
}