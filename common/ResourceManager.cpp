#include "../Resources/ResourceManager.h"

namespace Rscs 
{
	void log(const std::string &msg, Debug::InfoType type)
	{
		Debug::Log(msg, type, { "Resources", "IO" });
	}

	WriteTime getLastWriteTime(const Path &path)
	{
		return std::filesystem::last_write_time(path);
	}


	Manager::Manager()
	{ Locator::Add(Locator::CmdNode("Update", this, &Manager::update)); }

	Manager &Manager::Get()
	{
		static Manager instance;
		return instance;
	}

	FileRef Manager::getFile(const Path &path)
	{
		FileRef filePtr;
		if (auto &fileIter = loadedFiles.find(path); fileIter != loadedFiles.end())
		{
			filePtr = fileIter->second;
		}
		else
		{
			auto *file = new FileInfo(loadFile(path));
			filePtr = loadedFiles.emplace(path, file).first->second;
		}
		return filePtr;
	}

	FileInfo	Manager::loadFile(const Path &path)
	{
		FileInfo result;

		std::ifstream stream(path, std::ios::in);
		if (stream.is_open())
		{
			std::stringstream sstr;
			sstr << stream.rdbuf();

			result = FileInfo(path, sstr.str(), getLastWriteTime(path));

			stream.close();
		}
		else
		{
			log("Could not open file at " + path + ".", Debug::Error);
		}

		return result;
	}

	void Manager::update()
	{
		for (auto &filePair : loadedFiles)
		{
			auto &file = filePair.second;
			auto lastWriteTime = getLastWriteTime(file->Path_);
 			if (file->LastWriteTime == lastWriteTime) { continue; }

			*file = FileInfo(loadFile(file->Path_));

			// If the file changed, check for all resources if this file is a dependency 
			// and they need to be reinitialized.
			for(auto &resource : loadedResources)
			{
				if(resource.expired()) { continue; }
				ResourceRef<ResourceBase> ref = std::shared_ptr<ResourceBase>(resource);
				ref->onFileChanged(file->Path_);	
			}
		}
	}
}

/*
namespace Resource1
{
	PathID hashPath::operator()(const Path &path) const
	{
		return CRC32::Get(path.data(), path.size());
	}

	std::filesystem::file_time_type getLastWriteTime(const Path &path)
	{
		return std::filesystem::last_write_time(path);
	}
	

	Resource::Resource() 
	{
		
	}
	
	//template<typename ...Args>
	//void Resource::Setup(Args &...fileContents)
	//{
	//	...
	//}
	
	void Resource::onFileChanged(const ResourceDependency &path)
	{
		if (ext::contains(dependencies, path))
		{
			update();
		}
	}

	void Resource::update()
	{
		Setup(dependencies);
	}


	FileContent loader::Load(const Path &path)
	{
		auto &contents = loadedFiles.find(path);
		if (contents != loadedFiles.end())
		{
			return getContents(contents->second);
		}
		else
		{
			return loadFile(path);
		}
	}
	
	auto loader::createFileInfo(const Path &path, const FileContent &contents) const -> FileInfo
	{
		FileInfo info;
		info.Content = contents;
		info.Path_ = path;
		info.lastWriteTime = getLastWriteTime(path);
		return info;
	}
	
	FileContent &loader::getContents(FileInfo &info) const
	{ return info.Content; }
	
	FileContent loader::loadFile(const Path &path)
	{
		FileInfo result;
	
		std::ifstream stream(path, std::ios::in);
		if (stream.is_open())
		{
			std::stringstream sstr;
			sstr << stream.rdbuf();
	
			result = createFileInfo(path, sstr.str());
	
			stream.close();
		}
		else
		{
			Debug::Log("Could not open file at " + path + ".", Debug::Error, { "Resources", "IO" });
		}
	
		return getContents(loadedFiles.emplace(path, result).first->second);
	}
	
	Manager &Manager::Get()
	{
		static Manager instance;
		return instance;
	}

	Manager::Manager() : loader_(new loader()), monitor_(new monitor(*this, loader_))
	{

	}
	


	monitor::monitor(Manager &manager, std::shared_ptr<loader> loader_) 
		: manager(&manager), loader_(loader_)
	{ Locator::Add(Locator::CmdNode("Update", this, &monitor::update)); }

	void monitor::update()
	{
		for (auto &filePair : loader_->loadedFiles)
		{
			auto &file = filePair.second;
			if (file.lastWriteTime == getLastWriteTime(file.Path_)) { continue; }
	
			auto hash = hashPath()(file.Path_);
	
			// If the file changde, check for all resources if this file is a dependency 
			// and they need to be reinitialized.
			for(auto &resourceItem : manager->loadedIndices)
			{
				auto &resource = resourceItem.second;
				manager->loadedResources[resource]->onFileChanged(file);
			}
		}
	}
}*/