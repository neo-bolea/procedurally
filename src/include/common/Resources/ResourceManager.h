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

//TODO: 
// Add resource dependencies
//	Check for changes
//	Resource Allocation
//
/*
namespace Resources
{
	class Manager
	{
	public:
		static Manager &Get();

		template<typename T, typename ...Args>
		ResourceRef<T> Load(const Args &...paths);
	
	private:
		loader resourceLoader;
		monitor resourceMonitor;

		std::unordered_map<FileID, std::shared_ptr<Resource>, PassHasher<FileID>> loadedIndices;
	};

	Manager &Manager::Get()
	{
		static Manager instance;
		return instance;
	}

	template<typename T, typename ...Args>
	auto Manager::Load(const Args &...paths) -> ResourceRef<T>
	{
		static_assert(std::is_base_of_v<Resource, T>,
			"The requested resource must derive from Resources::Manager::Resource.");
	
		auto ids = { hashPath(paths)... };
		FileID summedID = std::accumulate(ids.begin(), ids.end(), 0);
	
		auto &rscIt = loadedIndices.find(summedID);
		if (rscIt == loadedIndices.end())
		{
			ResourceRef<T> resource = new T();
			resource->Setup({ { paths, resourceLoader.loadFile(paths).Contents }... });
	
			ResourceInfo info(static_cast<std::shared_ptr<Resource>>(resource), ids);
			rscIt = loadedIndices.emplace(summedID, info).first;
		}
		return dynamic_cast<ResourceRef<T>>(rscIt->second.resource)	
	}
}*/

/*
namespace Resource1
{
	using Path = std::string;
	using PathID = uint;
	using FileContent = std::string;
	class Resource;

	struct hashPath
	{
		PathID operator()(const Path &path) const;
	};


	std::filesystem::file_time_type getLastWriteTime(const Path &path);
	using WriteTime = decltype((getLastWriteTime)(Path()));

	struct FileInfo
	{
		Path Path_;
		FileContent Content;
		WriteTime lastWriteTime;

		FileInfo() {}
		FileInfo(FileContent content, Path path) : Content(content), Path_(path) {}
	};

	static bool operator ==(const FileInfo &lhs, const FileInfo &rhs) 
	{
		return lhs.Path_ == rhs.Path_;
	}

	using ResourceDependency = FileInfo;

	class Resource
	{
	public:
		Resource();

		//template<typename ...Args>
		//void Setup(Args &...fileContents);
		virtual void Setup(const std::vector<FileInfo> &files) = 0;

		void onFileChanged(const ResourceDependency &path);

	public:
		void update();

		std::vector<ResourceDependency> dependencies;
	};
	//template<typename ResourceType = Resource>
	//using ResourceRef = std::shared_ptr<ResourceType>;

	template<typename Type>
	std::shared_ptr<Type> makeRef(Type &resource)
	{ return std::make_shared<Type>(resource); }

	template<typename Type>
	std::shared_ptr<Type> getResource(std::shared_ptr<Resource> resource)
	{
		auto derivedResource = dynamic_cast<Type&>(*(resource.get()));
		return makeRef<Type>(derivedResource);
	}

	class loader;
	class Manager;
	class monitor;

	class loader
	{
	public:
		FileContent Load(const Path &path);

		std::unordered_map<Path, FileInfo> loadedFiles;

	private:
		FileInfo createFileInfo(const Path &path, const FileContent &contents) const;
		FileContent &getContents(FileInfo &info) const;
		FileContent loadFile(const Path &path);
	};

	class Manager
	{
	public:
		static Manager &Get();

		template<typename Type, typename ...Args>
		std::shared_ptr<Type> Load(Args &...paths);

	private:
		friend loader;
		friend monitor;

		Manager();

		template<typename Type>
		std::shared_ptr<Type> createResource(std::vector<Path> &paths)
		{
			Type resource;
			std::vector<FileInfo> files;
			for(auto &path : paths)
			{
				files.push_back({ loader_->Load(path), path });
			}
			resource.Setup(files);

			std::shared_ptr<Resource> rscRef = std::shared_ptr<Resource>(&dynamic_cast<Resource&>(resource));
			loadedIndices.emplace(paths, loadedIndices.size()).first->second; //Change to push back index instead
			return getResource<Type>(loadedResources.emplace_back(rscRef));

			//auto baseRsc = loadedResources[result->second].get();
			//auto rscResult = dynamic_cast<Type&>(*baseRsc);
			//return std::make_shared<Type>(rscResult);
		}

		std::shared_ptr<loader> loader_;
		std::unique_ptr<monitor> monitor_;

		using Hasher = AccumulateHasher<hashPath, std::vector<Path>, size_t>;
		std::unordered_map<std::vector<Path>, size_t, Hasher> loadedIndices;
		std::vector<std::shared_ptr<Resource>> loadedResources;

	};

	class monitor
	{
	public:
		monitor(Manager &manager, std::shared_ptr<loader> loader_);

	private:
		std::unique_ptr<Manager> manager;
		std::shared_ptr<loader> loader_;

		void update();
	};


	template<typename Type, typename ...Args>
	std::shared_ptr<Type> Manager::Load(Args &...paths)
	{
		static_assert(std::is_base_of_v<Resource, Type> && !std::is_same_v<Resource, Type>,
			"The resource to load must derive from Resource.");
		
		std::vector<Path> pathsV = { paths... };
		auto &result = loadedIndices.find(pathsV); 
		if(result != loadedIndices.end())
		{
			auto baseRsc = loadedResources[result->second].get();
			auto rscResult = dynamic_cast<Type&>(*baseRsc);
			return std::make_shared<Type>(rscResult);
		}
		else
		{
			return createResource<Type>(pathsV);
		}
	}
}

class StringWrapper : public Resource1::Resource
{
public:
	void Setup(const std::vector<Resource1::FileInfo> &files)
	{
		std::cout << files[0].Content << std::endl;
	}

	void Test()
	{
		std::cout << this->dependencies.size() << std::endl;
	}
};*/

/*template<typename T>
struct Resource;

struct ResourceManager
{
public:
	using FilePath = std::string;

	ResourceManager(size_t memorySize = 1024)
	{
		memoryPool = malloc(memorySize);
	}

	static ResourceManager &inst();

	template<typename T>
	Resource<T> &Load(FilePath &path);

	std::string LoadFile(FilePath &path) const;

private:
	friend struct ResourceContainerBase;
	template<typename T>
	friend struct ResourceContainer;

	using PoolID = size_t;
	using FileContents = std::string;

	void clearMemory(ResourceContainerBase &cont);
	void *getMemory(PoolID id);

	std::unordered_map<std::string, ResourceContainerBase *> resources;
	void *memoryPool;
	PoolID top;
};	

struct ResourceContainerBase
{
public:
	using FileContents = ResourceManager::FileContents;
	using PoolID = ResourceManager::PoolID;

	virtual PoolID Setup(FileContents contents, void *top) = 0;

protected:
	friend ResourceManager;

	ResourceContainerBase(ResourceManager &manager)
		: manager(manager)
	{
	
	}

	virtual void *createResource();
	void increaseRefCount() { refCount++; }
	void decreaseRefCount()
	{ 
		refCount--;
		manager.clearMemory(*this);
	}

	std::vector<ResourceContainerBase *> dependents;

	ResourceManager &manager;
	ResourceManager::PoolID id;
	uint refCount = 0;
};

template<typename T>
struct ResourceContainer : ResourceContainerBase
{
public:
	using ResourceContainerBase::ResourceContainerBase;

	virtual PoolID Setup(FileContents contents, void *top) 
	{
		if (resource != nullptr)
		{
			// Deallocate
		}
		resource = new/*(top)* / T(contents);
		return sizeof(T);
	}

private:
	friend ResourceManager;
	friend Resource<T>;

	T *resource = nullptr;

	virtual void *createResource();
};

template<typename T>
struct Resource
{
public:
	~Resource()
	{
		container.decreaseRefCount();
	}

	T &operator *()
	{
		return container.resource;
	}

private:
	friend ResourceContainerBase;

	Resource(ResourceContainer<T> &container) : container(container) 
	{
		container.increaseRefCount();
	}



	ResourceContainer<T> &container;
};*/