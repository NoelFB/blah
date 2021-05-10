#pragma once
#include <blah/streams/stream.h>
#include <blah/filesystem.h>

namespace Blah
{
	class FileStream : public Stream
	{
	public:
		FileStream();
		FileStream(const FilePath& path, FileMode mode);
		FileStream(FileStream&& fs) noexcept;
		FileStream& operator=(FileStream&& fs) noexcept;

		size_t length() const override;
		size_t position() const override;
		size_t seek(size_t position) override;
		bool is_open() const override;
		bool is_readable() const override;
		bool is_writable() const override;
		void close() override;

	protected:
		size_t read_data(void* ptr, size_t length) override;
		size_t write_data(const void* ptr, size_t length) override;

	private:
		FileMode m_mode;
		FileRef m_file;
	};
}