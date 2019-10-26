#pragma once

#include "cmdKeys.h"

#include "DesignPatterns.h"
#include "MathHelpers.h"

#include <assert.h>
#include <functional>
#include <tuple>
#include <vector>

// A static service locator that stores services through sequences of command keys (CmdKey).
//
// Services are added by passing a "tree", where each branch is accessed by the corresponding
// command key. Clients can then call the appropiate service by passing the correct sequence
// of command keys that reaches the expected leaf. 
// NOTE: The client has to be aware of the trees for the services to be able to use them.
// EXAMPLE:
//	CNTREE
//	(
//		CNSTR Input, 
//			Locator::CmdNode(SetKey,      this, &Inputs::keyPress),
//			Locator::CmdNode(SetButton,   this, &Inputs::mousePress),
//			Locator::CmdNode(SetMousePos, this, &Inputs::mouseMove),
//		CNEND
//	);
//
// Creates the following sequences:
// Input-SetKey => Inputs::keyPress()
// Input-SetButton => Inputs::mousePress()
// Input-SetMousePos => Inputs::mouseMove()

class Locator : public Singleton<Locator>
{
	using LeafFunc = std::function<void(void *, const type_info &)>;

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

	// Runtime, see above
	template<typename ...Args>
	void Call(const keySequences::id id, Args &&...args);

private:
	friend Singleton<Locator>;
	Locator() {}

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

private:
	friend Locator;

	struct nodePath;

	// Enables the use of a vector for functions with different signatures,
	// by bottlenecking to void *.
	template<typename Signature, typename ...FuncArgs>
	static void variableFunctionSignature(void *args, const type_info &info, Signature func);

	// Converts a tree of CmdNodes (nested) into a list of separate paths to every node in the tree.
	std::vector<nodePath> dissectTree();

	// Either contains nested objects or a callable
	LeafFunc leafFunc;
	std::vector<CmdNode> nodes;

	// The key that fits the current tree branch, to create the key sequence.
	CmdKey levelKey;
};

// Makes the interface for creating trees look more pleasant.
#define CNTREE(...) { using namespace CmdKeys; Locator::Get().Add(__VA_ARGS__); }0
#define CNTREE_END );
#define CNSTR Locator::CmdNode {
#define CNEND }

#include "cmdNode.inc"
#include "Locator.inc"