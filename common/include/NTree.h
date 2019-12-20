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

	void ClearCache();
	void Merge();

	int NodeCount();

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

	bool nodeExists(NodeIter &node, ChildIndex index);
	void canonalizeCoords(Coords &coords, size_t sizeMask);
	NodeCoords getNodeCoords(Coords &coords, uchar level);

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
void NTREE_SIGNATURE::ClearCache() { retrievalCache.clear(); }

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Merge() { merge(Nodes.find(1)); }

NTREE_TEMPLATE_SIGNATURE
int NTREE_SIGNATURE::NodeCount() { return nodeCount(Nodes.find(1)); }


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
		func(NodeInfo{ thisPos, thisSize, node->second });
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
			func(NodeInfo{ thisPos + childOffset, thisSize / 2, node->second });
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
bool NTREE_SIGNATURE::nodeExists(NodeIter &node, ChildIndex index)
{
	return (node->second.childExistMask >> index) & 1;
}

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

/*
#pragma once

#include "Debug.h"
#include "Vec.h"
#include "Util.h"

#include <cassert>
#include <unordered_map>
#include <vector>

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

template<typename T, uchar Levels, uchar Dimensions = 2>
class LinearTree
{
public:
	struct Node;
	using NodePtr = Node&;
	using ChildIndex = uchar;
	using NCode = uint;
	using Coord = uint;
	using Coords = Vec<Coord, Dimensions>;
	using NodeCoord = bool;
	using NodeCoords = Vec<NodeCoord, Dimensions>;
	using CacheIndex = uint;


	struct Node
	{
		Node(T data = T()) : Data(data) {}
		Node(const Node &node) : Data(node.Data) {}

		T Data;
	};

	struct NodeInfo
	{
		Coords Pos;
		size_t Size;
		T Value;
	};

	struct PassHash
	{
		uint operator()(const NCode &code) const
		{ return code; }
	};

public:
	LinearTree(T initial);

	const T &Get(Coords coords);
	void Set(Coords coords, T data);
	void SetDelayed(Coords coords, T data);

	void Map(function_view<void(NodeInfo)> func);

	void UpdateDelayed();
	void ClearCache();
	void Merge();

	int NodeCount();

	std::unordered_map<NCode, Node, PassHash> Nodes;
	int Size;

private:
	ChildIndex getRelLocCode(NodeCoords coords);
	NCode getLocCode(NCode parentCode, const NodeCoords &coords);
	NCode getLocCode(NCode parentCode, ChildIndex i);

	bool nodeExists(NCode code);
	Node getChildNode(NCode parCode, ChildIndex i);

	uint getCacheHash(const Coords &pos);

	void map(function_view<void(NodeInfo)> &func, NCode code, Coords nodePos, size_t nodeSize);
	bool merge(NCode code);
	int nodeCount(NCode code);


	const int childrenPerNode = std::pow(2, Dimensions);
	const NCode overMaxBit = 1 << ((Levels + 1) * Dimensions);

	std::vector<uint> pathsToUpdate;
	std::unordered_map<uint, T, PassHash> retrievalCache;
};


#define NTREE_TEMPLATE_SIGNATURE template<typename T, uchar Levels, uchar Dimensions>
#define NTREE_SIGNATURE LinearTree<T, Levels, Dimensions>

NTREE_TEMPLATE_SIGNATURE
NTREE_SIGNATURE::LinearTree(T initial)
{
	Size = std::pow(2, Levels);
	Nodes.reserve(MaxNodeCount(Levels, childrenPerNode) / 2);
	retrievalCache.reserve(MaxNodeCount(Levels, childrenPerNode) / 2);
	Nodes[1] = Node(initial);
}


NTREE_TEMPLATE_SIGNATURE
const T &NTREE_SIGNATURE::Get(Coords coords)
{
	//Check if the coordiantes were previously cached.
	//If they were, return their value.
	CacheIndex cacheHash = getCacheHash(coords);
	auto iter = retrievalCache.find(cacheHash);
	if (iter != retrievalCache.end()) { return iter->second; }

	NCode code = 1;
	uint size_mask = Size - 1;
	char level = Levels;

	while (true)
	{
		//Go down the tree and get the xyz for the next child node.
		size_mask >>= 1;
		level--;
		NodeCoords coords_;
		for (size_t i = 0; i < Dimensions; i++)
		{ coords_[i] = coords[i] >> level; }

		NCode child = getLocCode(code, coords_);

		//If the child exists (and size >= 1, checked in NodeExists())
		if (nodeExists(child))
		{
			code = child;

			for (size_t i = 0; i < Dimensions; i++)
			{ coords[i] &= size_mask; }
		}
		else
		{
			//Add the data to the cache and return.
			retrievalCache[cacheHash] = Nodes[code].Data;
			return Nodes[code].Data;
		}
	}
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Set(Coords coords, T data)
{
	NCode code = 1;
	Coord size_mask = Size - 1;
	char level = Levels;

	while (true)
	{
		//Go down the tree and get the xyz for the next child node.
		NodeCoords coords_;
		for (size_t i = 0; i < Dimensions; i++)
		{ coords_[i] = coords[i] >> (level - 1); }
		ChildIndex i = getRelLocCode(coords_);
		NCode child = getLocCode(code, i);

		//If the child exists (and size >= 1, checked in NodeExists())
		if (nodeExists(child))
		{
			//Get the next node and update xyz.
			code = getLocCode(code, i);

			size_mask >>= 1;
			level--;
			for (size_t i = 0; i < Dimensions; i++)
			{ coords[i] &= size_mask; }
		}
		else
		{
			//If we're on the last level (most detail), set the data.
			if (level == 0)
			{
				Nodes[code].Data = data;
				return;
			}
			else
			{	//Else, split the required child.
				Nodes[child] = Node(child);
				code = child;

				size_mask >>= 1;
				level--;
				for (size_t i = 0; i < Dimensions; i++)
				{ coords[i] &= size_mask; }
			}
		}
	}
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::SetDelayed(Coords coords, T data)
{
	NCode code = 1;
	uint size_mask = Size - 1;
	char level = Levels;

	//Generate the code for the node to be written to.
	while (level != 0)
	{
		//Go down the tree and get the xyz for the next child node.
		size_mask >>= 1;
		level--;
		NodeCoords coords_;
		for (size_t i = 0; i < Dimensions; i++)
		{ coords_[i] = coords[i] >> level; }

		//Get the child node.
		ChildIndex i = getRelLocCode(coords_);
		code = getLocCode(code, i);

		//Update xyz.
		for (size_t i = 0; i < Dimensions; i++)
		{ coords[i] &= size_mask; }
	}

	//Write to that node without creating it's parents.
	//Pushing the path will make UpdateDelayed() create the parents.
	pathsToUpdate.push_back(code);
	Nodes[code].Data = data;
}


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Map(function_view<void(NodeInfo)> func) { map(func, 1, Coords(), Size); }


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::UpdateDelayed()
{
	for (uint code : pathsToUpdate)
	{
		ChildIndex child;

		while (code != 1)
		{
			code = code >> Dimensions;
			if (!nodeExists(code))
			{ Nodes[code] = Node(); }
			else { break; }
		}
	}
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::ClearCache() { retrievalCache.clear(); }

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Merge() { merge(1); }

NTREE_TEMPLATE_SIGNATURE
int NTREE_SIGNATURE::NodeCount() { return nodeCount(1); }


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
bool NTREE_SIGNATURE::nodeExists(NCode code)
{ return !(code & overMaxBit) && Nodes.find(code) != Nodes.end(); }

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::getChildNode(NCode parCode, ChildIndex i) -> Node
{
	NCode code = getLocCode(parCode, i);
	return nodeExists(code) ? Nodes[code] : NULL;
}


NTREE_TEMPLATE_SIGNATURE
uint NTREE_SIGNATURE::getCacheHash(const Coords &pos)
{
	uint sum = 0;
	for (size_t i = 0; i < Dimensions; i++)
	{ sum |= (pos[i] << Levels * (Dimensions - i - 1)); }
	return sum;
}


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::map(function_view<void(NodeInfo)> &func, NCode thisCode, Coords thisPos, size_t thisSize)
{
	bool isParent = false;
	Coords childOffset;
	int currDim = 0;
	for (ChildIndex i = 0; i < childrenPerNode; i++)
	{
		NCode child = getLocCode(thisCode, i);
		if (nodeExists(child))
		{
			map(func, child, thisPos + childOffset, thisSize / 2);
			isParent = true;
		}

		childOffset[currDim] = (childOffset[currDim] + thisSize / 2) & (thisSize / 2);
		currDim = (currDim + 1) & (Dimensions - 1);
	}

	if (!isParent)
	{
		auto &node = Nodes.find(thisCode);
		func({ thisPos, thisSize, node->second.Data });
	}
}

NTREE_TEMPLATE_SIGNATURE
bool NTREE_SIGNATURE::merge(NCode code)
{
	T data = Nodes[code].Data;
	//TODO: Pick the correct type, based on how many children there are (e.g. 2D->4->uchar, 4D->16->ushort).
	uchar childrenExist = 0;

	//Try to merge all children (unless they are already leaves).
	bool canMerge = true, hasChildren = false, hasAllChildren = true;
	for (ChildIndex i = 0; i < childrenPerNode; i++)
	{
		NCode child = getLocCode(code, i);
		if (nodeExists(child))
		{
			hasChildren = true;
			childrenExist |= (1 << i);
			if (!merge(child)) { canMerge = false; }
		}
		else { hasAllChildren = false; }
	}

	//If this node is a leaf, it is already merged.
	if (!hasChildren) { return true; }

	//If they couldn't be merged, that means that not all data was the same,
	//	since the first false returned from Merge() must be because sameData == false.
	if (!canMerge) { return false; }

	//If the children can be merged, check if they all have the same data.
	bool sameData = true, firstChild = true;
	T tester;
	for (ChildIndex i = 0; i < childrenPerNode; i++)
	{
		if ((childrenExist >> i) & 1)
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
			if ((childrenExist >> i) & 1)
			{ Nodes.erase(getLocCode(code, i)); }
		}

		Nodes[code].Data = tester;
	}
	return sameData;
}

NTREE_TEMPLATE_SIGNATURE
int NTREE_SIGNATURE::nodeCount(NCode code)
{
	uint sum = 1;
	for (ChildIndex i = 0; i < childrenPerNode; i++)
	{
		NCode child = getLocCode(code, i);
		if (nodeExists(child))
		{ sum += nodeCount(child); }
	}
	return sum;
}
*/