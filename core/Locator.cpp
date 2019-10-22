#include "Systems/Locator.h"

Locator::CmdNode::CmdNode(CmdKey cmd, const CmdNode &inner) : levelKey(cmd)
{ nodes.push_back(inner); }


struct Locator::keySequenceInfo
{ std::vector<CmdKey> Keys; const LeafFunc Func; };

std::vector<Locator::keySequenceInfo> Locator::traverseTree(CmdNode &node)
{
	if(node.nodes.size() == 0) { return {{ { node.levelKey }, node.leafFunc }}; }
	std::vector<keySequenceInfo> cmdPaths;
	for(auto &currNode : node.nodes)
	{
		std::vector<keySequenceInfo> newPaths = traverseTree(currNode);
		for(auto &path : newPaths)
		{
			keySequenceInfo pathStart = { { node.levelKey }, path.Func };
			pathStart.Keys.insert(pathStart.Keys.end(), path.Keys.begin(), path.Keys.end());
			cmdPaths.push_back(pathStart);
		}
	}

	return cmdPaths;
}

void Locator::Add(CmdNode &tree)
{
	auto cmdPaths = traverseTree(tree);
	for(size_t i = 0; i < cmdPaths.size(); i++)
	{
		keySequences::id id = keySequences::toID(cmdPaths[i].Keys);
		cmds[id].push_back(cmdPaths[i].Func);
	}
}