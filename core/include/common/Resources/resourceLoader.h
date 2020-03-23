#pragma once 
/*<
#include "resourceTypes.h"

#include <filesystem>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>

namespace Resources
{
	struct FileContent 
	{
		std::string Contents; 
		std::filesystem::file_time_type TimeUpdated;
	};
	using FileContentRef = std::reference_wrapper<FileContent>;

	class loader
	{
	public:
		auto &loadFile(const FilePath &path);
	
		std::unordered_map<FilePath, FileContent> files;
	};
	
	auto &loader::loadFile(const FilePath &path)
	{
		auto &fileIt = files.find(path);
		if (fileIt == files.end())
		{
			FileContent result;

			std::ifstream stream(path, std::ios::in);
			if (stream.is_open())
			{
				std::stringstream sstr;
				sstr << stream.rdbuf();
	
				result.Contents = sstr.str();
				result.TimeUpdated = std::filesystem::last_write_time(path);
	
				stream.close();
			}
			else
			{
				Debug::Log("Could not open file at " + path + ".", Debug::Error, { "Resources", "IO" });
			}
	
			fileIt = files.emplace(path, result).first;
		}
	
		return fileIt->second;
	}
}*/