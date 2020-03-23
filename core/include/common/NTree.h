#pragma once

#include "Common/Debug.h"
#include "Common/Templates.h"
#include "Common/Vec.h"
#include "Common/Util.h"

#include <cassert>
#include <unordered_map>
#include <vector>

constexpr int MaxNodeCount
(const uint levels, const uint ChildrenPerNode)
{
	int total = 1, nodeCount = ChildrenPerNode;

	for(uint i = 0; i < levels; i++)
	{
		total += nodeCount;
		nodeCount *= ChildrenPerNode;
	}

	return total;
}

enum class CoordinateSystem { Resolution, Normalized };

template<typename T, uchar Levels, CoordinateSystem System, uchar Dimensions = 2>
class NTree 
{
private:
	template<CoordinateSystem>
	struct TreeCoordTypes 
	{ using type = uint; };
	
	template<>
	struct TreeCoordTypes<CoordinateSystem::Normalized> 
	{ using type = float; };

public:
	static const int Resolution = 2 << (Levels - 1);
	static const int ChildrenPerNode = 2 << (Dimensions - 1);

	struct Node;
	using ChildIndex = uchar;
	using NCode = uint;
	using Coord = typename TreeCoordTypes<System>::type;
	using Coords = Vec<Coord, Dimensions>;
	using NodeCoord = bool;
	using NodeCoords = Vec<NodeCoord, Dimensions>;
	using NeighborCoords = Vec<bool, Dimensions>;
	using CacheIndex = uint;
	using DataType = T;

	
	struct Node 
	{
		Node(DataType data = DataType()) : Data(data), exists(true) {}
		DataType Data;
		union
		{
			uchar childExistMask;
			bool isParent = false;
		};
		bool exists = false;
	};

	struct NodeInfo;

	NTree(DataType initial);

	template<bool UseCache = true>
	const DataType &Get(Coords coords);
	void Set(Coords coords, DataType data);

	void Map(function_view<void(NodeInfo)> func);
	void ForAllChildren(function_view<void(NodeInfo)> func, NodeInfo &code);
	void Merge();
	int NodeCount();

	void ClearCache();

	std::vector<Node> Nodes;
	using NodeIter = std::pair<NCode, Node>;
	void GetConnectionGraph(std::vector<NodeInfo> &vertices);

	Coord TreeSize();

private:
	template<REQUIRES(Dimensions == 2)>
	void getConnectionGraph
		(std::vector<NodeInfo> &vertices, NodeInfo &node);
	void getConnectionHorizontal
		(std::vector<NodeInfo> &vertices, NodeInfo &nodeA, NodeInfo &nodeB);
	void getConnectionVertictal
		(std::vector<NodeInfo> &vertices, NodeInfo &nodeA, NodeInfo &nodeB);

	void map(function_view<void(NodeInfo)> &func, NodeInfo &node);
	bool merge(NodeInfo &node);
	int nodeCount(NodeInfo &node);
	DataType foldAvg(NodeInfo &node);
	DataType getNodeNeighbor(NCode node, NeighborCoords &direction);

	auto &getRoot();

	Coords nodeCoordsToOffset(ChildIndex child, Coord levelSize) const;
	void transformCoordsToDeeperLevel(Coords &coords, size_t sizeMask) const;
	NodeCoords coordsToChildCoords(const Coords &coords, Coord levelSize) const;

	bool childExists(NodeInfo &node, ChildIndex index, NodeInfo &childNode);
	bool childExists(const Node &node, ChildIndex index) const;
	ChildIndex getChildIndex(const Coords &coords, Coord levelSize) const;

	NodeCoords childIndexToNodeCoords(ChildIndex index) const;
	ChildIndex nodeCoordToChildIndex(NodeCoords coords) const;
	NCode codeToChildCode(NCode parentCode, const NodeCoords &coords) const;
	NCode codeToChildCode(NCode parentCode, ChildIndex i) const;

	struct CacheHasher;
	std::unordered_map<uint, T, CacheHasher> retrievalCache;
};

#define NTREE_TEMPLATE_SIGNATURE template<typename T, uchar Levels, CoordinateSystem System, uchar Dimensions>
#define NTREE_SIGNATURE NTree<T, Levels, System, Dimensions>


NTREE_TEMPLATE_SIGNATURE
struct NTREE_SIGNATURE::NodeInfo
{
	NodeInfo() {}
	NodeInfo(NodeIter &node, NTREE_SIGNATURE *tree, Coords pos, Coord size) 
		: Pos(pos), Size(size)
	{
		if (node.exists)
		{
			this->code = node->first;
			this->node = &node->second;
			isValid = true;
		}
		else
		{ isValid = false; }
	}
	NodeInfo(NodeIter &node, NTREE_SIGNATURE *tree) 
		: NodeInfo(node, tree, Coords(), Coord()) {}

	NodeInfo(Node &node, NCode code, Coords pos, Coord size) 
		: node(&node), Pos(pos), Size(size), code(code) {}
	NodeInfo(Node &node, NCode code) 
		: NodeInfo(node, code, Coords(), Coord()) {}

	NodeInfo(NodeInfo &node, Coords pos, Coord size) 
		: node(node.node), Pos(pos), Size(size), code(node.code) {}
	NodeInfo(NodeInfo &node) 
		: NodeInfo(node, node.Pos, node.Size) {}


	NCode GetCode() { return code; }
	Node &GetNode() { return *node; }

	bool IsValid() { return isValid; }

	Coords Pos;
	Coord Size;

private:
	Node *node;
	NCode code;
	bool isValid = true;
};


NTREE_TEMPLATE_SIGNATURE
struct NTREE_SIGNATURE::CacheHasher
{
	uint operator()(const Coords &pos) const
	{
		uint sum = 0;
		for (size_t i = 0; i < Dimensions; i++)
		{ 
			if constexpr (System == CoordinateSystem::Resolution)
			{ sum |= (pos[i] << Levels * (Dimensions - i - 1)); }
			else if constexpr (System == CoordinateSystem::Normalized)
			{ sum |= (int(pos[i] * Resolution) << Levels * (Dimensions - i - 1)); }
		}
		return sum;
	}
};


NTREE_TEMPLATE_SIGNATURE
NTREE_SIGNATURE::NTree(DataType initial)
{
	Nodes.resize(MaxNodeCount(Levels, ChildrenPerNode)*4);
	//Nodes.reserve(MaxNodeCount(Levels, ChildrenPerNode) / 2);
	retrievalCache.reserve(MaxNodeCount(Levels, ChildrenPerNode) / 2);
	Nodes[1] = Node(initial);
}


NTREE_TEMPLATE_SIGNATURE
template<bool UseCache>
auto NTREE_SIGNATURE::Get(Coords coords) -> const DataType &
{
	if constexpr(UseCache)
	{
		// Check if the coordinates were previously cached.
		// If they were, return their value.
		auto cachedValue = retrievalCache.find(coords);
		if (cachedValue != retrievalCache.end()) { return cachedValue.GetNode(); }
	}

	int levelSize = Resolution;
	NodeInfo &thisNode = NodeInfo(getRoot(), 1);

	while (true)
	{
		NodeInfo childNode;
		ChildIndex index = getChildIndex(coords, levelSize);

		// If the child exists, travel to it.
		if (childExists(thisNode, index, childNode))
		{
			thisNode = childNode;

			// Make the coordinates relative to the next node.
			levelSize /= 2;
			transformCoordsToDeeperLevel(coords, levelSize - 1);
		}
		else
		{
			// If the child doesn't exist, return this node's value.
			if constexpr (UseCache)
			{ retrievalCache[coords] = thisNode.GetNode().Data; }
			return thisNode.GetNode().Data;
		}
	}
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Set(Coords coords, DataType data)
{
	char level = Levels;
	int levelSize = Resolution;

	NodeInfo &thisNode = NodeInfo(getRoot(), 1);

	while (true)
	{
		// Get the next node's coordinates inside this node.
		NodeCoords nodeCoords = coordsToChildCoords(coords, levelSize);
		ChildIndex relLocCode = nodeCoordToChildIndex(nodeCoords);
		NCode child = codeToChildCode(thisNode.GetCode(), relLocCode);

		// If we're on the last level (most detail), set the data.
		if (level == 0)
		{
			thisNode.GetNode().Data = data;
			return;
		}

		bool isLeaf = !thisNode.GetNode().isParent;
		bool createNextNode = false, sameData = false;

		// If this node is a leaf, split the node and travel to the new child.
		if (isLeaf)
		{
			if(thisNode.GetNode().Data == data)
			{ sameData = true; }
			else
			{ createNextNode = true; }
		}
		else
		{
			NodeInfo nextNode(Nodes[child], child);

			if (!nextNode.IsValid())
			{ createNextNode = true; }
			else
			{
				if(nextNode.GetNode().Data == data)
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
			thisNode.GetNode().childExistMask |= (1 << relLocCode);
			Nodes[child] = thisNode.GetNode().Data;
			Nodes[child].exists = true;
			thisNode = NodeInfo(Nodes[child], child);
		}

		// Make the coordinates relative to the next node.
		level--;
		levelSize /= 2;
		transformCoordsToDeeperLevel(coords, levelSize - 1);
	}
}


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Map(function_view<void(NodeInfo)> func) 
{ map(func, NodeInfo(getRoot(), 1, Coords(), TreeSize())); }


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Merge() { merge(NodeInfo(getRoot(), 1)); }

NTREE_TEMPLATE_SIGNATURE
int NTREE_SIGNATURE::NodeCount() { return nodeCount(NodeInfo(getRoot(), 1)); }


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::ClearCache() { retrievalCache.clear(); }


NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::TreeSize() -> Coord
{
	if constexpr (System == CoordinateSystem::Resolution)
	{ return Resolution; } 
	else if constexpr (System == CoordinateSystem::Normalized)
	{ return 1.0; }
}


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::GetConnectionGraph(std::vector<NodeInfo> &vertices)
{
	getConnectionGraph(vertices, NodeInfo(getRoot(), this, Coords(), TreeSize()));
}

NTREE_TEMPLATE_SIGNATURE
template<typename>
void NTREE_SIGNATURE::getConnectionGraph(
	std::vector<NodeInfo> &vertices, NodeInfo &node)
{
	if (!node.GetNode().isParent) { return; }

	Coords childOffset;
	for (ChildIndex i = 0; i < ChildrenPerNode; i++)
	{
		NodeInfo child;
		if (childExists(node, i, child))
		{
			childOffset = static_cast<Coords>(childIndexToNodeCoords(i)) * (node.Size / 2.);

			child.Pos = node.Pos + childOffset;
			child.Size = node.Size / 2;
			getConnectionGraph(vertices, child);
		}
	}

	NodeInfo bl = NodeInfo(Nodes.find(codeToChildCode(node.GetCode(), 0)), this, node.Pos + Coords(0, 0) * node.Size / 2, node.Size / 2);
	NodeInfo br = NodeInfo(Nodes.find(codeToChildCode(node.GetCode(), 1)), this, node.Pos + Coords(1, 0) * node.Size / 2, node.Size / 2);
	NodeInfo tl = NodeInfo(Nodes.find(codeToChildCode(node.GetCode(), 2)), this, node.Pos + Coords(0, 1) * node.Size / 2, node.Size / 2);
	NodeInfo tr = NodeInfo(Nodes.find(codeToChildCode(node.GetCode(), 3)), this, node.Pos + Coords(1, 1) * node.Size / 2, node.Size / 2);
	getConnectionHorizontal(vertices, bl, br);
	getConnectionHorizontal(vertices, tl, tr);
	getConnectionVertictal(vertices, bl, tl);
	getConnectionVertictal(vertices, br, tr);
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::getConnectionHorizontal(
	std::vector<NodeInfo> &vertices, NodeInfo &nodeA, NodeInfo &nodeB)
{
	if(!nodeA.IsValid() || !nodeB.IsValid()) { return; }

	NodeInfo brA(Nodes.find(codeToChildCode(nodeA.GetCode(), 1)), this, nodeA.Pos + Coords(1, 0) * nodeA.Size / 2, nodeA.Size / 2);
	NodeInfo trA(Nodes.find(codeToChildCode(nodeA.GetCode(), 3)), this, nodeA.Pos + Coords(1, 1) * nodeA.Size / 2, nodeA.Size / 2);
	NodeInfo blB(Nodes.find(codeToChildCode(nodeB.GetCode(), 0)), this, nodeB.Pos + Coords(0, 0) * nodeB.Size / 2, nodeB.Size / 2);
	NodeInfo tlB(Nodes.find(codeToChildCode(nodeB.GetCode(), 2)), this, nodeB.Pos + Coords(0, 1) * nodeB.Size / 2, nodeB.Size / 2);

	bool aPar = nodeA.GetNode().isParent;
	bool bPar = nodeB.GetNode().isParent;
	if(aPar)
	{
		if (bPar)
		{
			getConnectionHorizontal(vertices, brA, blB);
			getConnectionHorizontal(vertices, trA, tlB);
		}
		else
		{
			getConnectionHorizontal(vertices, brA, nodeB);
			getConnectionHorizontal(vertices, trA, nodeB);
		}
	}
	else if(bPar)
	{
		getConnectionHorizontal(vertices, nodeA, blB);
		getConnectionHorizontal(vertices, nodeA, tlB);
	}
	else
	{
		if(nodeA.GetNode().Data.Config != 2 && nodeB.GetNode().Data.Config != 2) { return; }
		vertices.push_back(NodeInfo(nodeA, nodeA.Pos + Coords(nodeA.Size / 2), nodeA.Size / 2));
		vertices.push_back(NodeInfo(nodeB, nodeB.Pos + Coords(nodeB.Size / 2), nodeB.Size / 2));
	}
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::getConnectionVertictal(
	std::vector<NodeInfo> &vertices, NodeInfo &nodeA, NodeInfo &nodeB)
{
	if(!nodeA.IsValid() || !nodeB.IsValid()) { return; }

	NodeInfo blA(Nodes.find(codeToChildCode(nodeA.GetCode(), 2)), this, nodeA.Pos + Coords(0, 1) * nodeA.Size / 2, nodeA.Size / 2);
	NodeInfo brA(Nodes.find(codeToChildCode(nodeA.GetCode(), 3)), this, nodeA.Pos + Coords(1, 1) * nodeA.Size / 2, nodeA.Size / 2);
	NodeInfo tlB(Nodes.find(codeToChildCode(nodeB.GetCode(), 0)), this, nodeB.Pos + Coords(0, 0) * nodeB.Size / 2, nodeB.Size / 2);
	NodeInfo trB(Nodes.find(codeToChildCode(nodeB.GetCode(), 1)), this, nodeB.Pos + Coords(1, 0) * nodeB.Size / 2, nodeB.Size / 2);

	bool aPar = nodeA.GetNode().isParent;
	bool bPar = nodeB.GetNode().isParent;
	if(aPar)
	{
		if (bPar)
		{
			getConnectionVertictal(vertices, blA, tlB);
			getConnectionVertictal(vertices, brA, trB);
		}
		else
		{
			getConnectionVertictal(vertices, blA, nodeB);
			getConnectionVertictal(vertices, brA, nodeB);
		}
	}
	else if(bPar)
	{
		getConnectionVertictal(vertices, nodeA, tlB);
		getConnectionVertictal(vertices, nodeA, trB);
	}
	else
	{
		if(nodeA.GetNode().Data.Config != 2 && nodeB.GetNode().Data.Config != 2) { return; }
		vertices.push_back(NodeInfo(nodeA, nodeA.Pos + Coords(nodeA.Size / 2), nodeA.Size / 2));
		vertices.push_back(NodeInfo(nodeB, nodeB.Pos + Coords(nodeB.Size / 2), nodeB.Size / 2));
	}
}


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::map(function_view<void(NodeInfo)> &func, NodeInfo &node)
{
	if (!node.GetNode().isParent)
	{ 
		func(NodeInfo(node, node.Pos, node.Size));
		return;
	}

	for (ChildIndex i = 0; i < ChildrenPerNode; i++)
	{
		Coords childOffset = nodeCoordsToOffset(i, node.Size/2);
		NodeInfo child;

		if (childExists(node, i, child))
		{
			NodeInfo childInfo(child, node.Pos + childOffset, node.Size / 2);
			map(func, childInfo);
		}
		else
		{
			Node newChildNode(node.GetNode().Data);
			NodeInfo childInfo(newChildNode, node.GetCode(), node.Pos + childOffset, node.Size / 2);
			func(NodeInfo(childInfo));
		}

		//childOffset[currDim] = (childOffset[currDim] + thisSize / 2) & (thisSize / 2);
		//currDim = (currDim + 1) & (Dimensions - 1);
	}
}

NTREE_TEMPLATE_SIGNATURE
bool NTREE_SIGNATURE::merge(NodeInfo &nodeInfo)
{
	//If this node is a leaf, it is already merged.
	if (!nodeInfo.GetNode().isParent) { return true; }

	//Try to merge all children (unless they are already leaves).
	bool canMerge = true, hasChildren = false, hasAllChildren = true;
	for (ChildIndex i = 0; i < ChildrenPerNode; i++)
	{
		NodeInfo child;
		if (childExists(nodeInfo, i, child) && !merge(child))
		{ canMerge = false; }
		else { hasAllChildren = false; }
	}

	//If they couldn't be merged, that means that not all data was the same,
	//	since the first false returned from Merge() must be because sameData == false.
	if (!canMerge) { return false; }

	//If the children can be merged, check if they all have the same data.
	bool sameData = true, firstChild = true;
	DataType tester, data = nodeInfo.GetNode().Data;
	for (ChildIndex i = 0; i < ChildrenPerNode; i++)
	{
		NodeInfo child;
		if (childExists(nodeInfo, i, child))
		{
			if (firstChild)
			{
				firstChild = false;
				tester = child.GetNode().Data;
				if (!hasAllChildren && data != tester) { sameData = false; break; }
			}
			else if (child.GetNode().Data != tester) { sameData = false; break; }
		}
	}

	//If all children have the same data, deallocate them and set the data of this tree.
	if (sameData)
	{
		for (uint i = 0; i < ChildrenPerNode; i++)
		{
			if (childExists(nodeInfo.GetNode(), i))
			{ Nodes[codeToChildCode(nodeInfo.GetCode(), i)].exists = false; }
		}
		nodeInfo.GetNode().isParent = false;
		nodeInfo.GetNode().Data = tester;
	}
	return sameData;
}

NTREE_TEMPLATE_SIGNATURE
int NTREE_SIGNATURE::nodeCount(NodeInfo &node)
{
	uint sum = 1;
	if(!node.GetNode().isParent)
	{ return sum; }

	for (ChildIndex i = 0; i < ChildrenPerNode; i++)
	{
		NodeInfo child;
		if (childExists(node, i, child))
		{ sum += nodeCount(child); }
	}
	return sum;
}

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::foldAvg(NodeInfo &node) -> DataType
{
	if (!node.GetNode().isParent) { return node.GetNode().Data; }

	DataType sum = DataType();
	int count = 0;

	for (size_t i = 0; i < ChildrenPerNode; i++)
	{
		NodeInfo child;
		if(childExists(node.GetNode(), i, child))
		{
			sum = sum + foldAvg(child);
			count++;
		}
	}

	return sum / count;
}

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::getNodeNeighbor(NCode code, NeighborCoords &direction) -> DataType
{
	if (direction.x == 0 && direction.y == 1)
	{
		if(!(code & 1))
		{
			NCode targetCode = code | 1;

			auto &targetNode = Nodes.find(targetCode);
			if(targetNode == Nodes.end())
			{ return Nodes.find(targetCode >> Dimensions).GetNode().Data; }
			else
			{ return foldAvg(targetNode); }
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
			return targetNode.GetNode().Data;
		}
	}
}


NTREE_TEMPLATE_SIGNATURE
auto &NTREE_SIGNATURE::getRoot()
{ return Nodes[1]; }

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::nodeCoordsToOffset(ChildIndex child, Coord levelSize) const -> Coords
{
	Coords offset;

	for (size_t j = 0; j < Dimensions; j++)
	{ offset[j] = ((child >> j) & 1) * levelSize; }

	return offset;
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::transformCoordsToDeeperLevel(Coords &coords, size_t sizeMask) const
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
auto NTREE_SIGNATURE::coordsToChildCoords(const Coords &coords, Coord levelSize) const -> NodeCoords
{
	NodeCoords childCoords;

	if constexpr (System == CoordinateSystem::Resolution)
	{
		for (size_t i = 0; i < Dimensions; i++)
		{ childCoords[i] = coords[i] >= levelSize; }
	}
	else if constexpr (System == CoordinateSystem::Normalized)
	{
		for (size_t i = 0; i < Dimensions; i++)
		{ childCoords[i] = coords[i] >= 0.5; }
	}

	return childCoords;
}

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::getChildIndex(const Coords &coords, Coord levelSize) const -> ChildIndex
{
	auto childCoords = coordsToChildCoords(coords, levelSize);
	return nodeCoordToChildIndex(childCoords);
}

NTREE_TEMPLATE_SIGNATURE
bool NTREE_SIGNATURE::childExists(
	NodeInfo &node,
	ChildIndex index,
	NodeInfo &childNode)
{ 
	if (childExists(node.GetNode(), index))
	{
		NCode childCode = codeToChildCode(node.GetCode(), index);
		childNode = NodeInfo(Nodes[childCode], childCode);
		return true;
	}
	else { return false; }
}

NTREE_TEMPLATE_SIGNATURE
bool NTREE_SIGNATURE::childExists(const Node &node, ChildIndex index) const
{ return (node.childExistMask >> index) & 1; }


NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::childIndexToNodeCoords(ChildIndex index) const -> NodeCoords
{
	NodeCoords coords;
	for (size_t j = 0; j < Dimensions; j++)
	{ coords[j] = ((index >> j) & 1); }
	return coords;
}

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::nodeCoordToChildIndex(NodeCoords coords) const -> ChildIndex
{
	ChildIndex sum = 0;
	for (size_t i = 0; i < Dimensions; i++)
	{ sum |= (coords[i] << (Dimensions - i - 1)); }
	return sum;
}

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::codeToChildCode(NCode parentCode, const NodeCoords &coords) const -> NCode
{ return (parentCode << Dimensions) | nodeCoordToChildIndex(coords); }

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::codeToChildCode(NCode parentCode, ChildIndex i) const -> NCode
{
	assert(i < ChildrenPerNode);
	return (parentCode << Dimensions) | i;
}


/*
#pragma once

#include "Common/Debug.h"
#include "Common/Vec.h"
#include "Common/Util.h"

#include <cassert>
#include <unordered_map>
#include <vector>

enum class CoordinateSystem { Resolution, Normalized };

template<typename T, uchar Levels, CoordinateSystem System, uchar Dimensions = 2>
class NTree 
{
private:
	template<CoordinateSystem>
	struct TreeCoordTypes 
	{ using type = uint; };
	
	template<>
	struct TreeCoordTypes<CoordinateSystem::Normalized> 
	{ using type = float; };

public:
	static const int Resolution = 2 << (Levels - 1);
	static const int ChildrenPerNode = 2 << (Dimensions - 1);

	struct Node;
	using ChildIndex = uchar;
	using NCode = uint;
	using Coord = typename TreeCoordTypes<System>::type;
	using Coords = Vec<Coord, Dimensions>;
	using NodeCoord = bool;
	using NodeCoords = Vec<NodeCoord, Dimensions>;
	using NeighborCoords = Vec<bool, Dimensions>;
	using CacheIndex = uint;
	using DataType = T;

	
	struct Node 
	{
		Node(DataType data = DataType()) : Data(data) {}
		DataType Data;
		union
		{
			uchar childExistMask;
			bool isParent = false;
		};
	};

	struct NodeInfo;

	NTree(DataType initial);

	template<bool UseCache = true>
	const DataType &Get(Coords coords);
	void Set(Coords coords, DataType data);

	void Map(function_view<void(NodeInfo)> func);
	void ForAllChildren(function_view<void(NodeInfo)> func, NodeInfo &code);
	void Expand();
	void Fill(function_view<DataType(Coords &)> func);
	void Merge();
	int NodeCount();

	using NodeIter = typename std::unordered_map<NCode, Node>::iterator;
	void GetConnectionGraph(std::vector<Coords> &vertices, std::vector<uint> &indices);

	Coord TreeSize();

protected:
	template<REQUIRES(Dimensions == 2)>
	void getConnectionGraph
		(std::vector<Coords> &vertices, std::vector<uint> &indices, NodeInfo &node);
	void getConnectionHorizontal
		(std::vector<Coords> &vertices, std::vector<uint> &indices, NodeInfo &nodeA, NodeInfo &nodeB);
	void getConnectionVertictal
		(std::vector<Coords> &vertices, std::vector<uint> &indices, NodeInfo &nodeA, NodeInfo &nodeB);

	void map(function_view<void(NodeInfo)> &func, NodeInfo &node);
	void expand(uint level, NodeInfo& node);
	bool merge(NodeInfo &node);
	int nodeCount(NodeInfo &node);
	DataType foldAvg(NodeInfo &node);
	DataType getNodeNeighbor(NCode node, NeighborCoords &direction);


	virtual NodeIter &setNode(NCode code, Node &node) = 0;
	virtual NodeIter &getNode(NCode code) = 0;

	virtual NodeIter &getRoot() = 0;

	virtual Coords nodeCoordsToOffset(ChildIndex child, Coord levelSize) const = 0;
	virtual void transformCoordsToDeeperLevel(Coords &coords, size_t sizeMask) const = 0;
	virtual NodeCoords coordsToChildCoords(const Coords &coords, Coord levelSize) const = 0;

	virtual bool getChild(NodeInfo &node, ChildIndex index, NodeInfo &childNode) = 0;
	virtual bool childExists(const Node &node, ChildIndex index) const = 0;
	virtual ChildIndex getChildIndex(const Coords &coords, Coord levelSize) const = 0;
	virtual ChildIndex nodeCoordToChildIndex(NodeCoords coords) const = 0;


	constexpr int MaxNodeCount(const uint levels, const uint ChildrenPerNode)
	{
		int total = 1, nodeCount = ChildrenPerNode;

		for(uint i = 0; i < levels; i++)
		{
			total += nodeCount;
			nodeCount *= ChildrenPerNode;
		}

		return total;
	}
};

#define NTREE_TEMPLATE_SIGNATURE template<typename T, uchar Levels, CoordinateSystem System, uchar Dimensions>
#define NTREE_SIGNATURE NTree<T, Levels, System, Dimensions>
#define LINEARTREE_SIGNATURE LinearTree<T, Levels, System, Dimensions>


NTREE_TEMPLATE_SIGNATURE
struct NTREE_SIGNATURE::NodeInfo
{
	NodeInfo() {}

	NodeInfo(NodeIter &node) 
		: NodeInfo(node->second, node->first, Coords(), Coord()) {}
	NodeInfo(NodeIter &node, Coords pos, Coord size) 
		: node(&node->second), Pos(pos), Size(size), code(node->first) {}

	NodeInfo(Node &node, NCode code, Coords pos, Coord size) 
		: node(&node), Pos(pos), Size(size), code(code) {}

	NodeInfo(NodeInfo &node, Coords pos, Coord size) 
		: node(node.node), Pos(pos), Size(size), code(node.code) {}
	NodeInfo(NodeInfo &node) 
		: NodeInfo(node, node.Pos, node.Size) {}


	NCode GetCode() { return code; }
	Node &GetNode() { return *node; }

	Coords Pos;
	Coord Size;

private:
	Node *node;
	NCode code;
};

template<typename T, uchar Levels, CoordinateSystem System, uchar Dimensions = 2>
struct LinearTree : NTree<T, Levels, System, Dimensions>
{
public:
	using Node = typename NTREE_SIGNATURE::Node;
	using typename NTREE_SIGNATURE::ChildIndex;
	using typename NTREE_SIGNATURE::NCode;
	using typename NTREE_SIGNATURE::Coord;
	using typename NTREE_SIGNATURE::Coords;
	using typename NTREE_SIGNATURE::NodeCoord;
	using typename NTREE_SIGNATURE::NodeCoords;
	using typename NTREE_SIGNATURE::NeighborCoords;
	using typename NTREE_SIGNATURE::CacheIndex;
	using typename NTREE_SIGNATURE::DataType;

	LinearTree(DataType initial) : NTree(initial)
	{ 
		Nodes.reserve(MaxNodeCount(Levels, ChildrenPerNode) / 2);
		retrievalCache.reserve(MaxNodeCount(Levels, ChildrenPerNode) / 2);
	}

	void ClearCache() { retrievalCache.clear(); }

private:
	NodeIter &setNode(NCode code, Node &node);
	NodeIter &getNode(NCode code);

	NodeIter &getRoot();

	Coords nodeCoordsToOffset(ChildIndex child, Coord levelSize) const;
	void transformCoordsToDeeperLevel(Coords &coords, size_t sizeMask) const;
	NodeCoords coordsToChildCoords(const Coords &coords, Coord levelSize) const;

	bool getChild(NodeInfo &node, ChildIndex index, NodeInfo &childNode);
	bool childExists(const Node &node, ChildIndex index) const;
	ChildIndex getChildIndex(const Coords &coords, Coord levelSize) const;


	ChildIndex nodeCoordToChildIndex(NodeCoords coords) const;
	NCode codeToChildCode(NCode parentCode, const NodeCoords &coords) const;
	NCode codeToChildCode(NCode parentCode, ChildIndex i) const;


	std::unordered_map<NCode, Node, PassHash<NCode>> Nodes;

	struct CacheHasher;
	std::unordered_map<uint, T, CacheHasher> retrievalCache;
};

NTREE_TEMPLATE_SIGNATURE
struct LINEARTREE_SIGNATURE::CacheHasher
{
	uint operator()(const Coords &pos) const
	{
		uint sum = 0;
		for (size_t i = 0; i < Dimensions; i++)
		{ 
			if constexpr (System == CoordinateSystem::Resolution)
			{ sum |= (pos[i] << Levels * (Dimensions - i - 1)); }
			else if constexpr (System == CoordinateSystem::Normalized)
			{ sum |= (int(pos[i] * Resolution) << Levels * (Dimensions - i - 1)); }
		}
		return sum;
	}
};


NTREE_TEMPLATE_SIGNATURE
NTREE_SIGNATURE::NTree(DataType initial)
{
	setNode(NodeInfo(getRoot()).GetCode(), Node(initial));
}


NTREE_TEMPLATE_SIGNATURE
template<bool UseCache>
auto NTREE_SIGNATURE::Get(Coords coords) -> const DataType &
{
	if constexpr(UseCache)
	{
		// Check if the coordinates were previously cached.
		// If they were, return their value.
		auto cachedValue = retrievalCache.find(coords);
		if (cachedValue != retrievalCache.end()) { return cachedValue.GetNode(); }
	}

	int levelSize = Resolution;
	NodeInfo &thisNode = NodeInfo(getRoot());

	while (true)
	{
		NodeInfo childNode;
		ChildIndex index = getChildIndex(coords, levelSize);

		// If the child exists, travel to it.
		if (getChild(thisNode, index, childNode))
		{
			thisNode = childNode;

			// Make the coordinates relative to the next node.
			levelSize /= 2;
			transformCoordsToDeeperLevel(coords, levelSize - 1);
		}
		else
		{
			// If the child doesn't exist, return this node's value.
			if constexpr (UseCache)
			{ retrievalCache[coords] = thisNode.GetNode().Data; }
			return thisNode.GetNode().Data;
		}
	}
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Set(Coords coords, DataType data)
{
	char level = Levels;
	int levelSize = Resolution;

	NodeInfo thisNode(getRoot());

	while (true)
	{
		// Get the next node's coordinates inside this node.
		NodeCoords nodeCoords = coordsToChildCoords(coords, levelSize);
		ChildIndex childIndex = nodeCoordToChildIndex(nodeCoords);
		NodeInfo child;

		// If we're on the last level (most detail), set the data.
		if (level == 0)
		{
			thisNode.GetNode().Data = data;
			return;
		}

		bool isLeaf = !thisNode.GetNode().isParent;
		bool createNextNode = false, sameData = false;

		// If this node is a leaf, split the node and travel to the new child.
		if (isLeaf)
		{
			if(thisNode.GetNode().Data == data)
			{ sameData = true; }
			else
			{ createNextNode = true; }
		}
		else
		{
			if(getChild(thisNode, childIndex, child))
			{
				if(child.GetNode().Data == data)
				{ sameData = true; }
				else
				{
					// Travel to the next node.
					thisNode = child;
				}
			}
			else
			{ createNextNode = true; }
		}

		// If the next node has the same data, there is no need to make changes.
		if (sameData) { return; }

		// If the next node doesn't exist, create it and travel to it.
		if (createNextNode)
		{
			thisNode.GetNode().childExistMask |= (1 << childIndex);
			auto &newNode = setNode(child.GetCode(), thisNode.GetNode());
			thisNode = NodeInfo(newNode);
		}

		// Make the coordinates relative to the next node.
		level--;
		levelSize /= 2;
		transformCoordsToDeeperLevel(coords, levelSize - 1);
	}
}


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Map(function_view<void(NodeInfo)> func) 
{
	NodeInfo rootInfo(getRoot(), Coords(), TreeSize());
	map(func, rootInfo);
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Expand() 
{ 
	Nodes.reserve(MaxNodeCount(Levels, ChildrenPerNode));
	expand(Levels, rootCode());
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Fill(function_view<DataType(Coords &)> func)
{
	Map([&](NodeInfo &info)
		{ 
			Set(info.Pos, func(info.Pos));
		});
	//Merge();
}


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::Merge() { merge(NodeInfo(getRoot())); }

NTREE_TEMPLATE_SIGNATURE
int NTREE_SIGNATURE::NodeCount() { return nodeCount(NodeInfo(getRoot())); }


NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::TreeSize() -> Coord
{
	if constexpr (System == CoordinateSystem::Resolution)
	{ return Resolution; } 
	else if constexpr (System == CoordinateSystem::Normalized)
	{ return 1.0; }
}


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::GetConnectionGraph(std::vector<Coords> &vertices, std::vector<uint> &indices)
{
	NodeInfo rootInfo(getRoot(), Coords(), TreeSize());
	getConnectionGraph(vertices, indices, rootInfo);
}

NTREE_TEMPLATE_SIGNATURE
template<typename>
void NTREE_SIGNATURE::getConnectionGraph(
	std::vector<Coords> &vertices, std::vector<uint> &indices, NodeInfo &node)
{
	if (!node.GetNode().isParent) { return; }

	Coords childOffset;
	for (ChildIndex i = 0; i < ChildrenPerNode; i++)
	{
		if (childExists(node.GetNode(), i))
		{
			for (size_t j = 0; j < Dimensions; j++)
			{ childOffset[j] = ((i >> j) & 1) * (node.Size / 2); }

			NCode childCode = codeToChildCode(node.GetCode(), i);
			getConnectionGraph(vertices, indices, NodeInfo(Nodes.find(childCode), this, node.Pos + childOffset, node.Size / 2.));
		}
	}

	NodeInfo bl = NodeInfo(Nodes.find(codeToChildCode(node.GetCode(), 0)), this, node.Pos + Coords(0, 0) * node.Size / 2, node.Size / 2);
	NodeInfo br = NodeInfo(Nodes.find(codeToChildCode(node.GetCode(), 1)), this, node.Pos + Coords(1, 0) * node.Size / 2, node.Size / 2);
	NodeInfo tl = NodeInfo(Nodes.find(codeToChildCode(node.GetCode(), 2)), this, node.Pos + Coords(0, 1) * node.Size / 2, node.Size / 2);
	NodeInfo tr = NodeInfo(Nodes.find(codeToChildCode(node.GetCode(), 3)), this, node.Pos + Coords(1, 1) * node.Size / 2, node.Size / 2);
	getConnectionHorizontal(vertices, indices, bl, br);
	getConnectionHorizontal(vertices, indices, tl, tr);
	getConnectionVertictal(vertices, indices, bl, tl);
	getConnectionVertictal(vertices, indices, br, tr);
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::getConnectionHorizontal(
	std::vector<Coords> &vertices, std::vector<uint> &indices, NodeInfo &nodeA, NodeInfo &nodeB)
{
	if(!nodeA.IsValid() || !nodeB.IsValid()) { return; }

	NodeInfo brA(Nodes.find(codeToChildCode(nodeA.GetCode(), 1)), this, nodeA.Pos + Coords(1, 0) * nodeA.Size / 2, nodeA.Size / 2);
	NodeInfo trA(Nodes.find(codeToChildCode(nodeA.GetCode(), 3)), this, nodeA.Pos + Coords(1, 1) * nodeA.Size / 2, nodeA.Size / 2);
	NodeInfo blB(Nodes.find(codeToChildCode(nodeB.GetCode(), 0)), this, nodeB.Pos + Coords(0, 0) * nodeB.Size / 2, nodeB.Size / 2);
	NodeInfo tlB(Nodes.find(codeToChildCode(nodeB.GetCode(), 2)), this, nodeB.Pos + Coords(0, 1) * nodeB.Size / 2, nodeB.Size / 2);

	bool aPar = nodeA.GetNode().isParent;
	bool bPar = nodeB.GetNode().isParent;
	if(aPar)
	{
		if (bPar)
		{
			getConnectionHorizontal(vertices, indices, brA, blB);
			getConnectionHorizontal(vertices, indices, trA, tlB);
		}
		else
		{
			getConnectionHorizontal(vertices, indices, brA, nodeB);
			getConnectionHorizontal(vertices, indices, trA, nodeB);
		}
	}
	else if(bPar)
	{
		getConnectionHorizontal(vertices, indices, nodeA, blB);
		getConnectionHorizontal(vertices, indices, nodeA, tlB);
	}
	else
	{
		if(nodeA.GetNode().Data.Config != 2 && nodeB.GetNode().Data.Config != 2) { return; }
		vertices.push_back(nodeA.Pos + Coords(nodeA.Size / 2));
		vertices.push_back(nodeB.Pos + Coords(nodeB.Size / 2));
	}
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::getConnectionVertictal(
	std::vector<Coords> &vertices, std::vector<uint> &indices, NodeInfo &nodeA, NodeInfo &nodeB)
{
	if(!nodeA.IsValid() || !nodeB.IsValid()) { return; }

	NodeInfo blA(Nodes.find(codeToChildCode(nodeA.GetCode(), 2)), this, nodeA.Pos + Coords(0, 1) * nodeA.Size / 2, nodeA.Size / 2);
	NodeInfo brA(Nodes.find(codeToChildCode(nodeA.GetCode(), 3)), this, nodeA.Pos + Coords(1, 1) * nodeA.Size / 2, nodeA.Size / 2);
	NodeInfo tlB(Nodes.find(codeToChildCode(nodeB.GetCode(), 0)), this, nodeB.Pos + Coords(0, 0) * nodeB.Size / 2, nodeB.Size / 2);
	NodeInfo trB(Nodes.find(codeToChildCode(nodeB.GetCode(), 1)), this, nodeB.Pos + Coords(1, 0) * nodeB.Size / 2, nodeB.Size / 2);

	bool aPar = nodeA.GetNode().isParent;
	bool bPar = nodeB.GetNode().isParent;
	if(aPar)
	{
		if (bPar)
		{
			getConnectionVertictal(vertices, indices, blA, tlB);
			getConnectionVertictal(vertices, indices, brA, trB);
		}
		else
		{
			getConnectionVertictal(vertices, indices, blA, nodeB);
			getConnectionVertictal(vertices, indices, brA, nodeB);
		}
	}
	else if(bPar)
	{
		getConnectionVertictal(vertices, indices, nodeA, tlB);
		getConnectionVertictal(vertices, indices, nodeA, trB);
	}
	else
	{
		if(nodeA.GetNode().Data.Config != 2 && nodeB.GetNode().Data.Config != 2) { return; }
		vertices.push_back(nodeA.Pos + Coords(nodeA.Size / 2));
		vertices.push_back(nodeB.Pos + Coords(nodeB.Size / 2));
	}
}


NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::map(function_view<void(NodeInfo)> &func, NodeInfo &node)
{
	if (!node.GetNode().isParent)
	{ 
		func(NodeInfo(node, node.Pos, node.Size));
		return;
	}

	for (ChildIndex i = 0; i < ChildrenPerNode; i++)
	{
		Coords childOffset = nodeCoordsToOffset(i, node.Size/2);
		NodeInfo child;

		if (getChild(node, i, child))
		{
			NodeInfo childInfo(child, node.Pos + childOffset, node.Size / 2);
			map(func, childInfo);
		}
		else
		{
			Node newChildNode(node.GetNode().Data);
			NodeInfo childInfo(newChildNode, node.GetCode(), node.Pos + childOffset, node.Size / 2);
			func(NodeInfo(childInfo));
		}

		//childOffset[currDim] = (childOffset[currDim] + thisSize / 2) & (thisSize / 2);
		//currDim = (currDim + 1) & (Dimensions - 1);
	}
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::expand(uint level, NodeInfo &node)
{
	for (ChildIndex i = 0; i < ChildrenPerNode; i++)
	{
		if (!childExists(node.GetNode(), i))
		{ 
			NCode childCode = codeToChildCode(node->first, i);
			NodeIter &newNode = Nodes.insert({ childCode, Node(node.GetNode().Data) }).first;
			node.GetNode().childExistMask |= (1 << i);
			if (level >= 1) { expand(level - 1, newNode); }
		}
	}
}

NTREE_TEMPLATE_SIGNATURE
bool NTREE_SIGNATURE::merge(NodeInfo &nodeInfo)
{
	//If this node is a leaf, it is already merged.
	if (!nodeInfo.GetNode().isParent) { return true; }

	//Try to merge all children (unless they are already leaves).
	bool canMerge = true, hasChildren = false, hasAllChildren = true;
	for (ChildIndex i = 0; i < ChildrenPerNode; i++)
	{
		NodeInfo child;
		if (getChild(nodeInfo, i, child))
		{ 
			if (!merge(child)) { canMerge = false; }
		}
		else { hasAllChildren = false; }
	}

	//If they couldn't be merged, that means that not all data was the same,
	//	since the first false returned from Merge() must be because sameData == false.
	if (!canMerge) { return false; }

	//If the children can be merged, check if they all have the same data.
	bool sameData = true, firstChild = true;
	DataType tester, data = nodeInfo.GetNode().Data;
	for (ChildIndex i = 0; i < ChildrenPerNode; i++)
	{
		NodeInfo child;
		if (getChild(nodeInfo, i, child))
		{
			if (firstChild)
			{
				firstChild = false;
				tester = child.GetNode().Data;
				if (!hasAllChildren && data != tester) { sameData = false; break; }
			}
			else if (child.GetNode().Data != tester) { sameData = false; break; }
		}
	}

	//If all children have the same data, deallocate them and set the data of this tree.
	if (sameData)
	{
		for (uint i = 0; i < ChildrenPerNode; i++)
		{
			if (childExists(nodeInfo.GetNode(), i))
			{ Nodes.erase(codeToChildCode(nodeInfo.GetCode(), i)); }
		}
		nodeInfo.GetNode().isParent = false;
		NodeInfo(getNode(nodeInfo.GetCode())).GetNode().Data = tester;
	}
	return sameData;
}

NTREE_TEMPLATE_SIGNATURE
int NTREE_SIGNATURE::nodeCount(NodeInfo &node)
{
	uint sum = 1;
	if(!node.GetNode().isParent)
	{ return sum; }

	for (ChildIndex i = 0; i < ChildrenPerNode; i++)
	{
		NodeInfo child;
		if (getChild(node.GetNode(), i, child))
		{ 
			sum += nodeCount(NodeInfo(child));
		}
	}
	return sum;
}

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::foldAvg(NodeInfo &node) -> DataType
{
	if (!node.GetNode().isParent) { return node.GetNode().Data; }

	DataType sum = DataType();
	int count = 0;

	for (size_t i = 0; i < ChildrenPerNode; i++)
	{
		if(childExists(node.GetNode(), i))
		{
			NCode childCode = codeToChildCode(node->first, i);
			sum = sum + foldAvg(Nodes.find(childCode));
			count++;
		}
	}

	return sum / count;
}

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::getNodeNeighbor(NCode code, NeighborCoords &direction) -> DataType
{
	if (direction.x == 0 && direction.y == 1)
	{
		if(!(code & 1))
		{
			NCode targetCode = code | 1;

			auto &targetNode = Nodes.find(targetCode);
			if(targetNode == Nodes.end())
			{ return Nodes.find(targetCode >> Dimensions).GetNode().Data; }
			else
			{ return foldAvg(targetNode); }
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
			return targetNode.GetNode().Data;
		}
	}
}


NTREE_TEMPLATE_SIGNATURE
auto LINEARTREE_SIGNATURE::setNode(NCode code, Node &node) -> NodeIter &
{ return Nodes.insert({ code, node }).first; }

NTREE_TEMPLATE_SIGNATURE
auto LINEARTREE_SIGNATURE::getNode(NCode code) -> NodeIter &
{ return Nodes.find(code); }

NTREE_TEMPLATE_SIGNATURE
auto LINEARTREE_SIGNATURE::getRoot() -> NodeIter &
{ return getNode(1); }

NTREE_TEMPLATE_SIGNATURE
auto NTREE_SIGNATURE::nodeCoordsToOffset(ChildIndex child, Coord levelSize) const -> Coords
{
	Coords offset;

	for (size_t j = 0; j < Dimensions; j++)
	{ offset[j] = ((child >> j) & 1) * levelSize; }

	return offset;
}

NTREE_TEMPLATE_SIGNATURE
void NTREE_SIGNATURE::transformCoordsToDeeperLevel(Coords &coords, size_t sizeMask) const
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
auto LINEARTREE_SIGNATURE::coordsToChildCoords(const Coords &coords, Coord levelSize) const -> NodeCoords
{
	NodeCoords childCoords;

	if constexpr (System == CoordinateSystem::Resolution)
	{
		for (size_t i = 0; i < Dimensions; i++)
		{ childCoords[i] = coords[i] >= levelSize; }
	}
	else if constexpr (System == CoordinateSystem::Normalized)
	{
		for (size_t i = 0; i < Dimensions; i++)
		{ childCoords[i] = coords[i] >= 0.5; }
	}

	return childCoords;
}

NTREE_TEMPLATE_SIGNATURE
auto LINEARTREE_SIGNATURE::getChildIndex(const Coords &coords, Coord levelSize) const -> ChildIndex
{
	auto childCoords = coordsToChildCoords(coords, levelSize);
	return nodeCoordToChildIndex(childCoords);
}

NTREE_TEMPLATE_SIGNATURE
bool LINEARTREE_SIGNATURE::getChild(
	NodeInfo &node,
	ChildIndex index,
	NodeInfo &childNode)
{ 
	if (childExists(node.GetNode(), index))
	{
		childNode = NodeInfo(Nodes.find(codeToChildCode(node.GetCode(), index)));
		return true;
	}
	else { return false; }
}

NTREE_TEMPLATE_SIGNATURE
bool LINEARTREE_SIGNATURE::childExists(const Node &node, ChildIndex index) const
{ return (node.childExistMask >> index) & 1; }


NTREE_TEMPLATE_SIGNATURE
auto LINEARTREE_SIGNATURE::nodeCoordToChildIndex(NodeCoords coords) const -> ChildIndex
{
	ChildIndex sum = 0;
	for (size_t i = 0; i < Dimensions; i++)
	{ sum |= (coords[i] << (Dimensions - i - 1)); }
	return sum;
}

NTREE_TEMPLATE_SIGNATURE
auto LINEARTREE_SIGNATURE::codeToChildCode(NCode parentCode, const NodeCoords &coords) const -> NCode
{ return (parentCode << Dimensions) | nodeCoordToChildIndex(coords); }

NTREE_TEMPLATE_SIGNATURE
auto LINEARTREE_SIGNATURE::codeToChildCode(NCode parentCode, ChildIndex i) const -> NCode
{
	assert(i < ChildrenPerNode);
	return (parentCode << Dimensions) | i;
}
*/