#include <memory>
#include <string>
#include <vector>

namespace IOUtil
{
	long GetFileSize(const char *filename);
	void WriteMemoryRaw(const char *filename, const char *data, size_t bytes);
	std::unique_ptr<char> ReadMemoryRaw(const char *filename, size_t &filesize);
	template<typename T>
	void ReadMemoryRaw(const char *filename, std::vector<T> &vec);
}

template<typename T>
void IOUtil::ReadMemoryRaw(const char *filename, std::vector<T> &vec)
{
	auto file = std::fstream(filename, std::ios::in | std::ios::binary);
	size_t filesize;
	file.read((char *)&filesize, sizeof(size_t));
	vec.resize(filesize / sizeof(T));
	file.read((char *)&vec[0], filesize);
	file.close();
}