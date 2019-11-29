// A static service locator that stores services through sequences of command keys (strings).
//
// Services are added by passing a "tree", where each branch is accessed by the corresponding
// command key (string). Clients can then call the appropiate service by passing the correct sequence
// of command keys that reaches the expected leaf. 
// NOTE: The client has to be aware of the trees for the services to be able to use them.
// EXAMPLE:
//	CNTREE
//	(
//		CNSTR "Input", 
//			Locator::CmdNode("SetKey",      this, &Inputs::keyPress),
//			Locator::CmdNode("SetMouseButton",   this, &Inputs::mousePress),
//			Locator::CmdNode("SetMousePos", this, &Inputs::mouseMove),
//		CNEND
//	);
//
// Creates the following sequences:
// "Input/SetKey" => Inputs::keyPress()
// "Input/SetButton" => Inputs::mousePress()
// "Input/SetMousePos" => Inputs::mouseMove()

#pragma once

#include "cmdKeys.h"

#include "DesignPatterns.h"
#include "MathHelpers.h"
#include "Templates.h"

#include <assert.h>
#include <functional>
#include <tuple>
#include <unordered_map>

static class Locator
{
	// Helper class to wrap functions with different signatures under the same signature,
	// using void * and type_info &. That way multiple functions can be saved in the same container.
	class LeafFunc;

public:
	// Helper class to construct a tree of functions that fit certain key sequences,
	// passed to the Locator.
	struct CmdNode;

	// Add functions to a given hash.
	static void Add(const CmdNode &tree);

	// Remove functions with a given hash.
	static void Remove(const CmdNode &tree);

	// Call all functions that fit the given hash, passing args as arguments.
	// Compile time
	template<size_t N, typename ...Args>
	static constexpr void Call(const char (&key)[N], Args &&...args);

	// Runtime, see above
	template<typename ...Args>
	static void Call(const locatorHasher::id id, Args &&...args);

private:
	// Erase all functions that where noted down by Remove().
	static void cleanRemoves();

	static int callStackSize;
	static std::unordered_multimap<locatorHasher::id, LeafFunc> cmds;
	static std::vector<decltype(cmds)::iterator> cmdsToRemove;
};

class Locator::LeafFunc
{
public:
	using FuncType = std::function<void(void *, size_t, const char *)>;

	LeafFunc() {}
	LeafFunc(FuncType func, void *funcPtr) : func(func), funcPtr(funcPtr) 
	{}

public:
	void *funcPtr;
	FuncType func;
};

#include "cmdNode.h"
#include "Locator.inc"