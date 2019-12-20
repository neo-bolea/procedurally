//#include "Systems/Resources.h"
//
//#include "Debug.h"
//#include "Hash.h"
//
//#include <fstream>
//#include <sstream>
//
//ResourceManager::ResourceManager(size_t startSize)
//{
//	yardBottom = malloc(startSize);
//}
//
//const ResourceManager::DataType *ResourceManager::Load(const std::string &path)
//{
//	stringHash pathHash = hash(path);
//	auto &file = files.find(pathHash);
//
//	if (file != files.end())
//	{
//		return &file->second; 
//	}
//	else
//	{
//		return loadTxtFile(path);
//	}
//}
//
//const ResourceManager::DataType *ResourceManager::loadTxtFile(const std::string &path)
//{
//	std::string data;
//	std::ifstream stream(path, std::ios::in);
//	if (stream.is_open())
//	{
//		std::stringstream sstr;
//		sstr << stream.rdbuf();
//		data = sstr.str();
//		stream.close();
//	}
//	else
//	{
//		Debug::Log("Could not open file at " + path, Debug::Error, { "Resources" });
//		return nullptr;
//	}
//
//	files.insert({ hash(path), data.c_str() });
//	return data;
//}
//
//void ResourceManager::storeToYard(void *toStore, size_t byteSize)
//{
//	if (std::distance(yardBottom, yardTop) + byteSize >= yardSize)
//	{
//		allocate();
//	}
//}
//
//void ResourceManager::allocate()
//{
//	void *tmp = malloc(yardSize * 2);
//	std::copy(yardBottom, yardTop, tmp);
//	free()
//}
//
//
//
//ResourceManager::stringHash ResourceManager::hash(const std::string &str)
//{ return CRC32::Get(str.data(), str.size()); }