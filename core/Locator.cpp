#include "Systems/Locator.h"
#include "Systems/cmdNode.h"

#include "StringUtils.h"

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

std::vector<Locator::CmdNode::nodePath> Locator::CmdNode::dissectTree()
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

void Locator::Add(CmdNode &tree)
{
	auto cmdPaths = tree.dissectTree();
	for(size_t i = 0; i < cmdPaths.size(); i++)
	{
		std::string joined = Strings::Join(cmdPaths[i].Keys, "/");
		locatorHasher::id id = locatorHasher::toID(joined.c_str(), joined.size());
		cmds.insert({id, cmdPaths[i].Func});
	}
}