#pragma once

#include "Locator.h"

// Helper class to construct a tree of functions that fit certain key sequences,
// passed to the Locator.
struct Locator::CmdNode
{
private:
	struct nodePath;

public:
	CmdNode();

	// Nest another object inside this one
	CmdNode(const char *cmd, const CmdNode &inner);

	// Nest multiple objects inside this one
	template<typename ...Args>
	CmdNode(const char *cmd, const Args &...inners);

	// Link a free function to this object
	template<typename ...FuncArgs>
	CmdNode(const char *cmd, void(*cb)(FuncArgs...));

	// Link a member function to this object
	template<typename ToBind, typename ...FuncArgs>
	CmdNode(const char *cmd, ToBind *toBind, void(ToBind::*cb)(FuncArgs...));

	// Converts a tree of CmdNodes (nested) into a list of separate paths to every node in the tree.
	std::vector<nodePath> dissectTree();

	// Either contains nested objects or a callable
	LeafFunc leafFunc;
	std::vector<CmdNode> nodes;

	// The key that fits the current tree branch, to create the key sequence.
	const char *levelKey;

private:
	// Enables the use of a vector for functions with different signatures,
	// by bottlenecking to void *.
	template<typename Signature, typename ...FuncArgs>
	static void variableFunctionSignature(void *args, const type_info &info, Signature func);
};

// Makes the interface for creating trees look more pleasant.
#define CNTREE(...) { Locator::Get().Add(__VA_ARGS__); }0
#define CNTREE_END );
#define CNSTR Locator::CmdNode {
#define CNEND }

#include "cmdNode.inc"