#include "Common/Systems/Locator/Locator.h"
#include "Common/Systems/Locator/cmdNode.h"

#include "Common/StringUtils.h"

#include <algorithm>

Locator::CmdNode::CmdNode() {}

Locator::CmdNode::CmdNode(const char *cmd, const CmdNode &inner) : levelKey(cmd)
{ nodes.push_back(inner); }


struct Locator::CmdNode::nodePath
{
	std::vector<const char *> Keys; 
	const LeafFunc Func; 

	nodePath(const char *key, LeafFunc func) 
		: Keys({key}), Func(func) 
	{}
};

std::vector<Locator::CmdNode::nodePath> Locator::CmdNode::dissectTree() const
{
	if(nodes.size() == 0) 
	{ 
		nodePath path(levelKey, leafFunc);
		return { path }; 
	}

	std::vector<nodePath> cmdPaths;
	for(auto &currNode : nodes)
	{
		std::vector<nodePath> newPaths = currNode.dissectTree();
		for(auto &path : newPaths)
		{
			nodePath pathStart(levelKey, path.Func);
			pathStart.Keys.insert(pathStart.Keys.end(), path.Keys.begin(), path.Keys.end());
			cmdPaths.push_back(pathStart);
		}
	}

	return cmdPaths;
}

int Locator::callStackSize = 0;
std::unordered_multimap<locatorHasher::id, Locator::LeafFunc> Locator::cmds;
std::vector<decltype(Locator::cmds)::iterator> Locator::cmdsToRemove;

void Locator::Add(const CmdNode &tree)
{
	auto cmdPaths = tree.dissectTree();
	for(size_t i = 0; i < cmdPaths.size(); i++)
	{
		std::string joined = Strings::Join(cmdPaths[i].Keys, "/");
		locatorHasher::id id = locatorHasher::toID(joined.c_str(), joined.size());
		cmds.insert({id, cmdPaths[i].Func});
	}
}

template<typename T, typename... U>
size_t getAddress(std::function<T(U...)> f) {
	typedef T(fnType)(U...);
	fnType ** fnPointer = f.template target<fnType*>();
	return (size_t) *fnPointer;
}

void Locator::Remove(const CmdNode &tree)
{
	auto cmdPaths = tree.dissectTree();
	for(size_t i = 0; i < cmdPaths.size(); i++)
	{
		std::string joined = Strings::Join(cmdPaths[i].Keys, "/");
		locatorHasher::id id = locatorHasher::toID(joined.c_str(), joined.size());
		
		auto itRange = cmds.equal_range(id);
		for(auto it = itRange.first; it != itRange.second; it++)
		{
			if(it->second.funcPtr == cmdPaths[i].Func.funcPtr)
			{
				cmdsToRemove.push_back(it);
			}
		}
	}
}

void Locator::cleanRemoves()
{
	for(size_t i = 0; i < cmdsToRemove.size(); i++)
	{
		cmds.erase(cmdsToRemove[i]);
	}
	cmdsToRemove.clear();
}