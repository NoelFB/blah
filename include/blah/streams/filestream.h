#pragma once
#include <blah/streams/stream.h>
#include <blah/core/filesystem.h>

namespace Blah
{
	class FileStream : public Stream
	{
	public:
		FileStream();
		FileStream(const FilePath& path, FileMode mode);
		FileStream(FileStream&& fs) noexcept;
		FileStream& operator=(FileStream&& fs) noexcept;
		~FileStream();

		i64 length() const override;
		i64 position() const override;
		i64 seek(i64 seekTo) override;
		bool is_open() const override;
		bool is_readable() const override;
		bool is_writable() const override;
		void close() override;

	protected:
		i64 read_into(void* ptr, i64 length) override;
		i64 write_from(const void* ptr, i64 length) override;

	private:
		FileMode m_mode;
		void*    m_handle;
	};
}