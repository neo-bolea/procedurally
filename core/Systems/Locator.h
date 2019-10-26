#pragma once

#include "cmdKeys.h"

#include "DesignPatterns.h"
#include "MathHelpers.h"
#include "Preprocessor.h"
#include "Templates.h"

#include <assert.h>
#include <functional>
#include <iostream>
#include <tuple>
#include <vector>

using LeafFunc = std::function<void(void *, const type_info &)>;

#define CNTREE(...) { using namespace CmdKeys; Locator::Get().Add(__VA_ARGS__); }0
#define CNTREE_END );
#define CNSTR Locator::CmdNode {
#define CNEND }


class Locator : public Singleton<Locator>
{
public:
	// Helper class to construct a tree of functions that fit certain key sequences,
	// passed to the Locator.
	struct CmdNode;

	// Add functions to given key sequences.
	void Add(CmdNode &tree);

	// Call all functions that fit the given key sequence, passing args as arguments.
	// Compile time
	template<size_t SIZE, typename ...Args>
	constexpr void Call(const CmdKey (&keys)[SIZE], Args &&...args);

	// Runtime
	template<typename ...Args>
	void Call(const keySequences::id id, Args &&...args);

private:
	friend Singleton<Locator>;
	Locator() {}

	struct keySequenceInfo;
	std::vector<keySequenceInfo> traverseTree(CmdNode &node);

	std::vector<LeafFunc> cmds[keySequences::largestPossibleBit()];
};


struct Locator::CmdNode
{
public:
	// Nest another object inside this one
	CmdNode(CmdKey cmd, const CmdNode &inner);

	// Nest multiple objects inside this one
	template<typename ...Args>
	CmdNode(CmdKey cmd, const Args &...inners);

	// Link a free function to this object
	template<typename ...FuncArgs>
	CmdNode(CmdKey cmd, void(*cb)(FuncArgs...));

	// Link a member function to this object
	template<typename Member, typename ...FuncArgs>
	CmdNode(CmdKey cmd, Member *toBind, void(Member::*cb)(FuncArgs...));


	// Either contains nested objects or a callable
	LeafFunc leafFunc;
	std::vector<CmdNode> nodes;

	// The key that fits the current tree branch, to create the key sequence.
	CmdKey levelKey;

private:
	// Enables the use of a vector for functions with different signatures,
	// by bottlenecking to void *.
	template<typename Signature, typename ...FuncArgs>
	static void variableFunctionSignature(void *args, const type_info &info, Signature func);
};

#include "cmdNode.inc"
#include "Locator.inc"