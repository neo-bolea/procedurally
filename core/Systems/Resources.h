//#include <string>
//#include <unordered_map>
//
//class ResourceManager
//{
//public:
//	using DataType = int *;
//
//	ResourceManager(size_t startSize = 1);
//
//	const DataType *Load(const std::string &path);
//
//private:
//	using stringHash = uint;
//	stringHash hash(const std::string &str);
//
//	const DataType *loadTxtFile(const std::string &path);
//	void storeToYard(void *toStore, size_t byteSize);
//	void allocate();
//
//	std::unordered_map<stringHash, DataType> files;
//	DataType yardBottom, yardTop;
//	size_t yardSize = 0;
//};