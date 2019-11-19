#pragma once

#include "MathHelpers.h"
#include "Hash.h"

#include "StaticStr.h"

#include <functional>
/*
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

//namespace sstr = ak_toolkit::static_str;
//
//template<size_t T>
//using CmdKey = sstr::string<T, sstr::char_array>;
//
//constexpr CmdKey xd = "xd" + sstr::literal("nigga");

using CmdKey = CmdKeys::CmdKey;*/


// ORs all levels of keys together, after they have been lshfted to the correct level.
//This guarantees uniqueness of all combination of keys (as long as they are inserted in 
//the correct level).
//Example: { <first val in level 1>, <third val in level 2>}, 
//where amount of vals in level 2 = 5 
//1 = 0b1, 3 = 0b10 
//0b01 << 0 = 0b01, 0b10 << (MSB of 5 = 3) = 0b11000, 
//id = 0b01 | 0b11000 = 0b11001 = 25 => done!

using CmdKey = char *;

class cmdKeySequenceHelper
{
private:
	friend class Locator;

	using id = uint64;

	//// Main Functions ////
	template<size_t SIZE>
	static constexpr id toID(const char *key);
	static id toID(const char *key, size_t size);

	/*// The largest bit a key sequence can contain.
	static constexpr uint largestPossibleValue();

	struct helpers
	{
	private:
		friend cmdKeySequenceHelper;

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
	};*/
};

#include "cmdKeys.inc"