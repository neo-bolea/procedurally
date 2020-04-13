#pragma once

#include "Common/Debug.h"
#include "Common/Util.h"
#include "Common/Hash.h"

#include "Common/Systems/Locator/Locator.h"
#include "Common/Templates.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>

namespace Rscs
{
	using FileContent = std::string;
	using Path = std::string;
	using PathID = uint;
	using WriteTime = std::filesystem::file_time_type;

	void log(const std::string &msg, Debug::InfoType type = Debug::Error);
	WriteTime getLastWriteTime(const Path &path);

	struct FileInfo
	{
		Path Path_;
		FileContent Contents;
		WriteTime LastWriteTime;

		FileInfo() {}
		FileInfo(const Path &path, FileContent &&contents)
			: FileInfo(path, std::forward<FileContent>(contents), WriteTime()) {}
		FileInfo(const Path &path, FileContent &&contents, WriteTime lastWriteTime)
			: Path_(path), Contents(contents), LastWriteTime(lastWriteTime) {}
	};
	using FileRef = std::shared_ptr<FileInfo>;

	struct ResourceBase
	{
		friend class Manager;

	protected:
		virtual void setup(std::unordered_set<FileRef> &files) = 0;

		bool previouslyInitialized = false;

	private:
		virtual void onFileChanged(const Path &path) final
		{
			if(std::find_if(dependencies.begin(), dependencies.end(),
				[&](const FileRef &info) { return info->Path_ == path; }) != dependencies.end())
			{
				setup(dependencies); 
			}
		}

		virtual void initialize(std::unordered_set<FileRef> &files) final
		{
			dependencies.reserve(files.size());
			for(auto &file : files)
			{
				dependencies.insert(file);
			}
			setup(files);
			previouslyInitialized = true;
		}

		std::unordered_set<FileRef> dependencies;
	};
	template<typename ResourceType>
	using ResourceRef = std::shared_ptr<ResourceType>;

	class Manager
	{
	public:
		static Manager &Get();

		template<typename ResourceType>
		ResourceRef<ResourceType> loadResource(const std::vector<std::string> &paths);

	private:
		Manager();


		void update();

		FileRef getFile(const Path &path);
		FileInfo loadFile(const Path &path);

		std::unordered_map<Path, FileRef> loadedFiles;
		std::vector<std::weak_ptr<ResourceBase>> loadedResources;
	};
}



namespace Rscs
{
	template<typename ResourceType>
	ResourceRef<ResourceType> Manager::loadResource(const std::vector<std::string> &paths)
	{
		std::unordered_set<FileRef> files(paths.size());
		for(auto &path : paths)
		{
			files.insert(getFile(path));			
		}

		ResourceType *resource = new ResourceType();
		resource->initialize(files);

		ResourceRef<ResourceType> rscRef = ResourceRef<ResourceType>(resource);
		loadedResources.push_back(rscRef);
		return ResourceRef<ResourceType>(rscRef);
	}
}