#pragma once
#include <blah/streams/stream.h>
#include <blah/filesystem.h>

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

		virtual int64_t length() const override;
		virtual int64_t position() const override;
		virtual int64_t seek(int64_t seekTo) override;
		virtual bool is_open() const override { return m_handle != nullptr; }
		virtual bool is_readable() const override { return m_handle != nullptr && (m_mode == FileMode::ReadWrite || m_mode == FileMode::Read); }
		virtual bool is_writable() const override { return m_handle != nullptr && (m_mode == FileMode::ReadWrite || m_mode == FileMode::Write); }
		virtual void close() override;

	protected:
		virtual int64_t read_into(void* ptr, int64_t length) override;
		virtual int64_t write_from(const void* ptr, int64_t length) override;

	private:
		FileMode	m_mode;
		void*		m_handle;
	};
}