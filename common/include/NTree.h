#pragma once

#include "Debug.h"
#include "Vec.h"
#include "Util.h"

#include <cassert>
#include <unordered_map>
#include <vector>

//TODO: Make Marching Squares work on a per 2x2 cells instead of inside each cell (as that creates duplicates).

static constexpr int MaxNodeCount(const uint levels, const uint childrenPerNode)
{
	int total = 1, nodeCount = childrenPerNode;

	for(uint i = 0; i < levels; i++)
	{
		total += nodeCount;
		nodeCount *= childrenPerNode;
	}

	return total;
}

enum class CoordinateSystem { Resolution, Normalized };

template<typename T, uchar Levels, CoordinateSystem System, uchar Dimensions = 2>
class LinearTree 
{
private:
	template<CoordinateSystem>
	struct TreeCoordTypes 
	{ using type = uint; };
	
	template<>
	struct TreeCoordTypes<CoordinateSystem::Normalized> 
	{ using type = float; };

public:
	struct Node;
	using ChildIndex = uchar;
	using NCode = uint;
	using Coord = typename TreeCoordTypes<System>::type;
	using Coords = Vec<Coord, Dimensions>;
	using NodeCoord = bool;
	using NodeCoords = Vec<NodeCoord, Dimensions>;
	using NeighborCoords = Vec<bool, Dimensions>;
	using CacheIndex = uint;

	
	struct Node 
	{
		Node(T data = T()) : Data(data) {}

		T Data;

		union
		{
			uchar childExistMask;
			bool isParent = false;
		};
	};

	struct NodeInfo
	{
		Coords Pos;
		Coord Size;
		Node &Node;
		T NeighborData;
	};

	struct PassHash
	{
		uint operator()(const NCode &code) const
		{ return code; }
	};


	LinearTree(T initial);

	template<bool UseCache = true>
	const T &Get(Coords coords);
	void Set(Coords coords, T data);

	void Map(function_view<void(NodeInfo)> func);
	void Expand();
	void Fill(function_view<T(Coords &)> func);
	void Merge();
	int NodeCount();

	void ClearCache();

	std::unordered_map<NCode, Node, PassHash> Nodes;
	int Size;

private:
	using NodeIter = decltype(Nodes.find(-1));


	ChildIndex getRelLocCode(NodeCoords coords);
	NCode getLocCode(NCode parentCode, const NodeCoords &coords);
	NCode getLocCode(NCode parentCode, ChildIndex i);

	uint getCacheHash(const Coords &pos);

	void map(function_view<void(NodeInfo)> &func, NodeIter &node, Coords thisPos, Coord thisSize);
	void expand(uint level, NodeIter& node);
	bool merge(NodeIter &node);
	int nodeCount(NodeIter &node);
	T foldAvg(NodeIter &node);

	bool nodeExists(NodeIter &node, ChildIndex index);
	void canonalizeCoords(Coords &coords, size_t sizeMask);
	NodeCoords getNodeCoords(Coords &coords, uchar level);

	T getNodeNeighbor(NCode node, NeighborCoords &direction);

	const int childrenPerNode = std::pow(2, Dimensions);
	const NCode overMaxBit = 1 << ((Levels + 1) * Dimensions);

	std::unordered_map<uint, T, PassHash> retrievalCache;
};


#define NTREE_TEMPLATE_SIGNATURE template<typename T, uchar Levels, CoordinateSystem System, uchar Dimensions>
#define NTREE_SIGNATURE LinearTree<T, Levels, System, Dimensions>

NTREE_TEMPLATE_SIGNATURE
NTREE_SIGNATURE::LinearTree(T initial)
{
	Size = std::pow(2, Levels);
	Nodes.reserve(MaxNodeCount(Levels, childrenPerNode) / 2);
	retrievalCache.reserve(MaxNodeCount(Levels, childrenPerNode) / 2);
	Nodes[1] = Node(initial);
}


NTREE_TEMPLATE_SIGNATURE
template<bool UseCache>
const T &NTREE_SIGNATURE::Get(Coords coords)
{
	CacheIndex cacheHash;
	if constexpr(UseCache)
	{
		// Check if the coordinates were previously cached.
		// If they were, return their value.
		cacheHash = getCacheHash(coords);
		auto cachedValue = retrievalCache.find(cacheHash);
		if (cachedValue != retrievalCache.end()) { return cachedValue->second; }
	}

	uint sizeMask = Size - 1;
	char level = Levels - 1;
	NodeIter &thisNode = Nodes.find(1);

	while (true)
	{
		// Get the next node's coordinates inside this node.
		NodeCoords nodeCoords;
		for (size_t i = 0; i < Dimensions; i++)
		{ nodeCoords[i] = coords[i] >= 0.5; }

		ChildIndex relLocCode = getRelLocCode(nodeCoords);

		// If the child exists, travel to it.
		if (nodeExists(thisNode, relLocCode))
		{
			NCode childCode = getLocCode(thisNode->first, relLocCode);
			thisNode = Nodes.find(getLocCode(thisNode->first, relLocCode));

			// Make the coordinates relative to the next node.
			sizeMask >>= 1;
			level--;

			canonalizeCoords(coords, sizeMask);
		}
		else
		{
			// If the child doesn't exist, return this node's value.
			if constexpr (UseCache)
			{ retrievalCache[cacheHash] = thisNode->second.Data; }
			return thisNode->second.Data;
		}
	}
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Set(Coords coords, T data)
{
	uint sizeMask = Size - 1;
	char level = Levels;

	NodeIter &thisNode = Nodes.find(1);

	while (true)
	{
		// Get the next node's coordinates inside this node.
		NodeCoords nodeCoords = getNodeCoords(coords, level);
		ChildIndex relLocCode = getRelLocCode(nodeCoords);
		NCode child = getLocCode(thisNode->first, relLocCode);

		// If we're on the last level (most detail), set the data.
		if (level == 0)
		{
			thisNode->second.Data = data;
			return;
		}

		bool isLeaf = !thisNode->second.isParent;
		bool createNextNode = false, sameData = false;

		// If this node is a leaf, split the node and travel to the new child.
		if (isLeaf)
		{
			if(thisNode->second.Data == data)
			{ sameData = true; }
			else
			{ createNextNode = true; }
		}
		else
		{
			auto &nextNode = Nodes.find(child);

			if (nextNode == Nodes.end())
			{ createNextNode = true; }
			else
			{
				if(nextNode->second.Data == data)
				{ sameData = true; }
				else
				{
					// Travel to the next node.
					thisNode = nextNode;
				}
			}
		}

		// If the next node has the same data, there is no need to make changes.
		if (sameData) { return; }

		// If the next node doesn't exist, create it and travel to it.
		if (createNextNode)
		{
			thisNode->second.childExistMask |= (1 << relLocCode);
			thisNode = Nodes.insert({ child, thisNode->second.Data }).first;
		}

		// Make the coordinates relative to the next node.
		sizeMask >>= 1;
		level--;
		canonalizeCoords(coords, sizeMask);
	}
}


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Map(function_view<void(NodeInfo)> func) 
{
	Coord startSize;
	if constexpr (System == CoordinateSystem::Resolution)
	{ startSize = Size; }
	else if constexpr (System == CoordinateSystem::Normalized)
	{ startSize = 1.0; }
	
	map(func, Nodes.find(1), Coords(), startSize);
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Expand() 
{ 
	Nodes.reserve(MaxNodeCount(Levels, childrenPerNode));
	expand(Levels, Nodes.find(1));
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Fill(function_view<T(Coords &)> func)
{
	Map([&](NodeInfo &info)
		{ 
			Set(info.Pos, func(info.Pos));
		});
	//Merge();
}


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Merge() { merge(Nodes.find(1)); }

NTREE_TEMPLATE_SIGNATURE
int NTREE_SIGNATURE::NodeCount() { return nodeCount(Nodes.find(1)); }


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::ClearCache() { retrievalCache.clear(); }


NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::getRelLocCode(NodeCoords coords) -> ChildIndex
{
	ChildIndex sum = 0;
	for (size_t i = 0; i < Dimensions; i++)
	{ sum |= (coords[i] << (Dimensions - i - 1)); }
	return sum;
}

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::getLocCode(NCode parentCode, const NodeCoords &coords) -> NCode
{ return (parentCode << Dimensions) | getRelLocCode(coords); }

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::getLocCode(NCode parentCode, ChildIndex i) -> NCode
{
	assert(i < childrenPerNode);
	return (parentCode << Dimensions) | i;
}


NTREE_TEMPLATE_SIGNATURE
uint NTREE_SIGNATURE::getCacheHash(const Coords &pos)
{
	uint sum = 0;
	for (size_t i = 0; i < Dimensions; i++)
	{ 
		if constexpr (System == CoordinateSystem::Resolution)
		{ sum |= (pos[i] << Levels * (Dimensions - i - 1)); }
		else if constexpr (System == CoordinateSystem::Normalized)
		{ sum |= (int(pos[i] * Size) << Levels * (Dimensions - i - 1)); }
	}

	return sum;
}


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::map(function_view<void(NodeInfo)> &func, NodeIter &node, Coords thisPos, Coord thisSize)
{
	Coords childOffset;
	int currDim = 0;

	if (!node->second.isParent)
	{ 
		func(NodeInfo{ thisPos, thisSize, node->second/*, getNodeNeighbor(node->first, NeighborCoords(0, 1))*/ });
		return;
	}

	for (ChildIndex i = 0; i < childrenPerNode; i++)
	{
		for (size_t j = 0; j < Dimensions; j++)
		{
			childOffset[j] = ((i >> j) & 1) * (thisSize / 2);
		}

		if (nodeExists(node, i))
		{
			NCode childCode = getLocCode(node->first, i);
			map(func, Nodes.find(childCode), thisPos + childOffset, thisSize / 2);
		}
		else
		{
			NCode childCode = getLocCode(node->first, i);
			func(NodeInfo{ thisPos + childOffset, thisSize / 2, node->second, getNodeNeighbor(childCode, NeighborCoords(0, 1)) });
		}

		//childOffset[currDim] = (childOffset[currDim] + thisSize / 2) & (thisSize / 2);
		//currDim = (currDim + 1) & (Dimensions - 1);
	}
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::expand(uint level, NodeIter &node)
{
	for (ChildIndex i = 0; i < childrenPerNode; i++)
	{
		if (!nodeExists(node, i))
		{ 
			NCode childCode = getLocCode(node->first, i);
			NodeIter &newNode = Nodes.insert({ childCode, Node(node->second.Data) }).first;
			node->second.childExistMask |= (1 << i);
			if (level >= 1) { expand(level - 1, newNode); }
		}
	}
}

NTREE_TEMPLATE_SIGNATURE
bool NTREE_SIGNATURE::merge(NodeIter &node)
{
	T data = node->second.Data;
	NCode code = node->first;

	//If this node is a leaf, it is already merged.
	if (!node->second.isParent) { return true; }

	//Try to merge all children (unless they are already leaves).
	bool canMerge = true, hasChildren = false, hasAllChildren = true;
	for (ChildIndex i = 0; i < childrenPerNode; i++)
	{
		if (nodeExists(node, i))
		{
			NCode childCode = getLocCode(code, i);
			NodeIter &child = Nodes.find(childCode);
			if (!merge(child)) { canMerge = false; }
		}
		else { hasAllChildren = false; }
	}

	//If they couldn't be merged, that means that not all data was the same,
	//	since the first false returned from Merge() must be because sameData == false.
	if (!canMerge) { return false; }

	//If the children can be merged, check if they all have the same data.
	bool sameData = true, firstChild = true;
	T tester;
	for (ChildIndex i = 0; i < childrenPerNode; i++)
	{
		if (nodeExists(node, i))
		{
			if (firstChild)
			{
				firstChild = false;
				tester = Nodes[getLocCode(code, i)].Data;
				if (!hasAllChildren && data != tester) { sameData = false; break; }
			}
			else if (Nodes[getLocCode(code, i)].Data != tester) { sameData = false; break; }
		}
	}

	//If all children have the same data, deallocate them and set the data of this tree.
	if (sameData)
	{
		for (uint i = 0; i < childrenPerNode; i++)
		{
			if (nodeExists(node, i))
			{ Nodes.erase(getLocCode(code, i)); }
		}
		node->second.isParent = false;

		Nodes[code].Data = tester;
	}
	return sameData;
}

NTREE_TEMPLATE_SIGNATURE
int NTREE_SIGNATURE::nodeCount(NodeIter &node)
{
	uint sum = 1;
	if(!node->second.isParent)
	{ return sum; }

	for (ChildIndex i = 0; i < childrenPerNode; i++)
	{
		if (nodeExists(node, i))
		{ 
			NCode childCode = getLocCode(node->first, i);
			sum += nodeCount(Nodes.find(childCode));
		}
	}
	return sum;
}

NTREE_TEMPLATE_SIGNATURE
T NTREE_SIGNATURE::foldAvg(NodeIter &node)
{
	if (!node->second.isParent) { return node->second.Data; }

	T sum = T();
	int count = 0;

	for (size_t i = 0; i < childrenPerNode; i++)
	{
		if(nodeExists(node, i))
		{
			NCode childCode = getLocCode(node->first, i);
			sum = sum + foldAvg(Nodes.find(childCode));
			count++;
		}
	}

	return sum / count;
}


NTREE_TEMPLATE_SIGNATURE
bool NTREE_SIGNATURE::nodeExists(NodeIter &node, ChildIndex index)
{ return (node->second.childExistMask >> index) & 1; }

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::canonalizeCoords(Coords &coords, size_t sizeMask)
{
	if constexpr (System == CoordinateSystem::Resolution)
	{
		for (size_t i = 0; i < Dimensions; i++)
		{ coords[i] &= sizeMask; }
	}
	else if constexpr (System == CoordinateSystem::Normalized)
	{
		for (size_t i = 0; i < Dimensions; i++)
		{
			if (coords[i] >= 0.5) { coords[i] -= 0.5; }
			coords[i] = coords[i] * 2.0;
		}
	}
}

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::getNodeCoords(Coords &coords, uchar level) -> NodeCoords
{
	NodeCoords nodeCoords;
	if constexpr (System == CoordinateSystem::Resolution)
	{
		for(size_t i = 0; i < Dimensions; i++)
		{ nodeCoords[i] = coords[i] >> (level - 1); }
	}
	else if constexpr (System == CoordinateSystem::Normalized)
	{
		for (size_t i = 0; i < Dimensions; i++)
		{ nodeCoords[i] = coords[i] >= 0.5; }
	}
	return nodeCoords;
}

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::getNodeNeighbor(NCode code, NeighborCoords &direction) -> T
{
	if (direction.x == 0 && direction.y == 1)
	{
		if(!(code & 1))
		{
			NCode targetCode = code | 1;

			auto &targetNode = Nodes.find(targetCode);
			if(targetNode == Nodes.end())
			{ return Nodes.find(targetCode >> Dimensions)->second.Data; }
			else
			{ 
				return foldAvg(targetNode); 
			}
		} 
		else
		{
			NCode targetCode = code;
			int currIndex = 0;//Dependent
			while (targetCode & (1 << currIndex))
			{
				targetCode &= ~(1 << currIndex);
				currIndex += Dimensions;
			}
			targetCode |= (1 << currIndex);

			auto &targetNode = Nodes.find(targetCode);
			while(targetNode == Nodes.end())
			{ 
				targetCode >>= Dimensions; 
				targetNode = Nodes.find(targetCode);
			}
			return targetNode->second.Data;
		}
	}
}