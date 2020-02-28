//#include "Resources.h"






/*
static ResourceManager::Get() &ResourceManager::Get()::inst()
{
	static ResourceManager::Get() instance;
	return instance;
}

template<typename T>
Resource<T> &ResourceManager::Get()::Load(FilePath &path)
{
	auto &rscIt = resources.find(path);
	if (rscIt != resources.end())
	{
		return *static_cast<Resource<T> *>(rscIt->second.createResource());
	}
	else
	{
		auto rsc = resources.emplace(path, ResourceContainer<T>(*this)).first->second;
		auto contents = LoadFile(path);
		top += rsc.Setup(contents, getMemory(top));
		return *static_cast<Resource<T>*>(rsc.createResource());
	}
}

std::string ResourceManager::Get()::LoadFile(FilePath &path) const
{
	std::string result;
	std::ifstream stream(path, std::ios::in);
	if (stream.is_open())
	{
		std::stringstream sstr;
		sstr << stream.rdbuf();
		result = sstr.str();
		stream.close();
	}
	else
	{
		Debug::Log("Could not open file at " + path, Debug::Error, { "Resources" });
		return "";
	}

	return result;
}

void ResourceManager::Get()::clearMemory(ResourceContainerBase &cont)
{
	for(auto &it : resources)
	{
		if (cont.id == it.second.id)
		{
			resources.erase(it.first);
			return;
		}
	}
}

void *ResourceManager::Get()::getMemory(PoolID id)
{
	return ((int8 *)memoryPool) + id;
}


template<typename T>
virtual void *ResourceContainer::createResource()
{
	return Resource<T>(*this);
}*/