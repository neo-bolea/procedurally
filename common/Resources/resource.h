#pragma once 

/*
#include "resourceTypes.h"

#include <string>
#include <vector>
namespace Resources
{
	using DependencyID = FileID;
	struct Resource
	{
		struct FileArg
		{
			std::string Path;
			const std::string &Contents;
		};
	
		void Setup(const std::vector<FileArg> &files)
		{
			for (size_t i = 0; i < files.size(); i++)
			{
				paths.push_back(hashPath(files[i].Path));
			}
			initialize(files);
		}
	
	protected:
		virtual void initialize(const std::vector<FileArg> &files) = 0;
	
	private:
		friend class Monitor;

		std::vector<FileID> paths;
	};

	template<typename ResourceType = Resource>
	using ResourceRef = ResourceType *;
}

struct StringWrapper : Resources::Resource
{
	virtual void initialize(const std::vector<FileArg> &files)
	{
		string = files[0].Contents;
	}

	std::string string;
};*/