#pragma once
#include <blah/streams/stream.h>

namespace Blah
{
	// Memory Stream moves over an existing buffer.
	// The Buffer must exist for the duration of the Memory Stream.
	class MemoryStream : public Stream
	{
	public:
		MemoryStream();
		MemoryStream(char* data, size_t length);
		MemoryStream(MemoryStream&& ms) noexcept;
		MemoryStream& operator=(MemoryStream&& ms) noexcept;
		~MemoryStream() override { m_data = nullptr; m_length = m_position = 0; }

		size_t length() const override;
		size_t position() const override;
		size_t seek(size_t seekTo) override;
		bool is_open() const override;
		bool is_readable() const override;
		bool is_writable() const override;
		void close() override;

		char* data();
		const char* data() const;

	protected:
		size_t read_data(void* ptr, size_t length) override;
		size_t write_data(const void* ptr, size_t length) override;

	private:
		char* m_data;
		size_t m_length;
		size_t m_position;
	};
}