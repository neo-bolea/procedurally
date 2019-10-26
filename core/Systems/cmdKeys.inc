#pragma once

template<size_t SIZE, size_t I>
constexpr keySequences::id keySequences::toID(const CmdKey (&keys)[SIZE])
{ 
	static_assert(SIZE <= MAX_CMDKEY_LEVELS, 
					  "The amount of commands cannot be greater than MAX_CMDKEY_LEVELS.");

	CmdKey thisKey = keys[(SIZE - 1) - I];
	id thisLevelsBits = thisKey; 

	// Subtract this level's first cmd's value to get a relative value
	const uint cmdLevelBase = helpers::keyNumOffsets[(SIZE - 1) - I]; 
	thisLevelsBits -= cmdLevelBase;

	// The amount this level has to be lshifted by
	const uint amountToShift = helpers::keyLevelBitOffset((SIZE - 1) - I);
	thisLevelsBits <<= amountToShift;

	if constexpr(I == SIZE - 1)
	{ 
		// Base case
		return thisLevelsBits;
	}
	else
	{
		// Recursive case
		id nextLevelsBits = toID<SIZE, I + 1>(keys);
		return thisLevelsBits | nextLevelsBits; 
	}
}

constexpr uint keySequences::largestPossibleBit()
{ return 1 << keySequences::helpers::keyLevelBitOffset(MAX_CMDKEY_LEVELS - 1); }

constexpr uint keySequences::helpers::keyLevelBitOffset(const uint I) 
{ 
	if(I == 0)
	{ 
		// Base case (first level doesn't have to be lshifted)
		return 0;
	}
	else
	{ 
		// Recursive case
		const uint levelCmdCount = keyNumOffsets[I] - keyNumOffsets[I - 1];
		const uint thisLevelsLShift = Math::MSB(levelCmdCount);
		const uint lastLevelsShift = keyLevelBitOffset(I - 1); 
		return thisLevelsLShift + lastLevelsShift;
	}
}