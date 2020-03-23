#pragma once 
/*
#include "resourceTypes.h"
#include "resourceLoader.h"

#include "./../Locator/Locator.h"

#include <filesystem>

namespace Resources
{
	class monitor
	{
	public:
		monitor()
		{
			Locator::Add(Locator::CmdNode("Update", this, &monitor::update));
		}

	private:
		void update();

		loader &loader_;
	};

	void monitor::update()
	{
		for(auto &it : loader_.files)
		{
			if (std::filesystem::last_write_time(it.first) == it.second.TimeUpdated)
			{
				continue;
			}

			FileID id = hashPath(it.first);
			for (auto &resourceIt : loader.loadedResources)
			{
				auto &resourceInfo = resourceIt.second;
			
				bool mustBeModified = false;
				for (auto &path : resourceInfo.paths)
				{
					if (path == id)
					{
						mustBeModified = true;
						break;
					}
				}
			
				if (mustBeModified)
				{
					std::vector<Resource::FileArg> args;
			
					for (auto &dependency1 : resourceInfo.dependencies)
					{
						args.emplace_back({ dependency1, loader.fileResources[dependency1].Contents });
					}
			
					resourceInfo.resource->Setup(args);
				}
			}
		}
	}
}*/