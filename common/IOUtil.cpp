#include "IOUtil.h"

#include <fstream>

namespace IOUtil
{
	long GetFileSize(const char *filename)
	{
		struct stat stat_buf;
		int rc = stat(filename, &stat_buf);
		return rc == 0 ? stat_buf.st_size : -1;
	}

	void WriteMemoryRaw(const char *filename, const char *data, size_t bytes)
	{
		auto file = std::fstream(filename, std::ios::out | std::ios::binary);
		file.write((char *)&bytes, sizeof(size_t));
		file.write(data, bytes);
		file.close();
	}

	std::unique_ptr<char> ReadMemoryRaw(const char *filename, size_t &filesize)
	{
		char *data;

		auto file = std::fstream(filename, std::ios::in | std::ios::binary);
		file.read((char *)&filesize, sizeof(size_t));
		data = (char *)malloc(filesize);
		file.read(data, filesize);
		file.close();

		return std::unique_ptr<char>(data);
	}
}