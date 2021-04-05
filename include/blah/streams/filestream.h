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

		virtual i64 length() const override;
		virtual i64 position() const override;
		virtual i64 seek(i64 seekTo) override;
		virtual bool is_open() const override;
		virtual bool is_readable() const override;
		virtual bool is_writable() const override;
		virtual void close() override;

	protected:
		virtual i64 read_into(void* ptr, i64 length) override;
		virtual i64 write_from(const void* ptr, i64 length) override;

	private:
		FileMode m_mode;
		void*    m_handle;
	};
}