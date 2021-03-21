#pragma once
#include <blah/streams/stream.h>
#include <blah/core/filesystem.h>

namespace Blah
{
	class FileStream : public Stream
	{
	public:
		FileStream();
		FileStream(const char* path, FileMode mode = FileMode::ReadWrite);
		FileStream(FileStream&& fs) noexcept;
		FileStream& operator=(FileStream&& fs) noexcept;
		~FileStream();

		virtual i64 length() const override;
		virtual i64 position() const override;
		virtual i64 seek(i64 seekTo) override;
		virtual bool is_open() const override { return m_handle != nullptr; }
		virtual bool is_readable() const override { return m_handle != nullptr && (m_mode == FileMode::ReadWrite || m_mode == FileMode::Read); }
		virtual bool is_writable() const override { return m_handle != nullptr && (m_mode == FileMode::ReadWrite || m_mode == FileMode::Write); }
		virtual void close() override;

	protected:
		virtual i64 read_into(void* ptr, i64 length) override;
		virtual i64 write_from(const void* ptr, i64 length) override;

	private:
		FileMode	m_mode;
		void*		m_handle;
	};
}