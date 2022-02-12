#pragma once
#include <blah/streams/stream.h>

namespace Blah
{
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
}