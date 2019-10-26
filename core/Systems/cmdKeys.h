#pragma once

#include "MathHelpers.h"

#include <functional>

namespace CmdKeys
{
	enum CmdKey
	{
		//Primary commands
		GameStart,
		Update,
		Input,

		PRIMARY_MAX,

		//Secondary commands
		SetKey,
		GetKey,
		SetButton,
		GetButton,
		SetMousePos,
		GetMousePos,

		SECONDARY_MAX,

		//Tertiary commands
		TERTIARY_MAX,
		QUATERNARY_MAX,
	};
};

const uint MAX_CMDKEY_LEVELS = 4;
using CmdKey = CmdKeys::CmdKey;


/* ORs all levels of keys together, after they have been lshfted to the correct level.
This guarantees uniqueness of all combination of keys (as long as they are inserted in 
the correct level).
Example: { <first val in level 1>, <third val in level 2>}, 
where amount of vals in level 2 = 5 
1 = 0b1, 3 = 0b10 
0b01 << 0 = 0b01, 0b10 << (MSB of 5 = 3) = 0b11000, 
id = 0b01 | 0b11000 = 0b11001 = 25 => done! */
class keySequences
{
private:
	friend class Locator;

	using id = uint64;

	//// Main Functions ////
	template<size_t SIZE, size_t I>
	static constexpr id toID(const CmdKey (&keys)[SIZE]);
	static id toID(const std::vector<CmdKey> &keys);

	// The largest bit a key sequence can contain.
	static constexpr uint largestPossibleBit();

	struct helpers
	{
	private:
		friend keySequences;

		// For calculating how many bits each level of keys has to be lshfted by.
		static constexpr uint keyLevelBitOffset(const uint I);

		// The value at which each key level starts.
		constexpr static uint keyNumOffsets[MAX_CMDKEY_LEVELS] = 
		{ 
			0,
			static_cast<uint>(CmdKey::PRIMARY_MAX),
			static_cast<uint>(CmdKey::SECONDARY_MAX),
			static_cast<uint>(CmdKey::TERTIARY_MAX) 
		};
	};
};

#include "cmdKeys.inc"