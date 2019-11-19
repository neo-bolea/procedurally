#pragma once

#include "cmdKeys.h"

#include "DesignPatterns.h"
#include "MathHelpers.h"
#include "Templates.h"

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
	template<size_t N, typename ...Args>
	constexpr void Call(const char (&key)[N], Args &&...args);

	// Runtime, see above
	template<typename ...Args>
	void Call(const cmdKeySequenceHelper::id id, Args &&...args);

private:
	friend Singleton<Locator>;
	Locator() {}

	//std::vector<LeafFunc> cmds[cmdKeySequenceHelper::largestPossibleValue()];
	std::unordered_multimap<cmdKeySequenceHelper::id, LeafFunc> cmds;
};

#include "cmdNode.h"
#include "Locator.inc"