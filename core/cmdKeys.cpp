#include "Systems\cmdKeys.h"

keySequences::id keySequences::toID(const std::vector<CmdKey> &keys)
{ 
	id sequenceID = 0;
	for(size_t i = 0; i < keys.size(); i++)
	{ 
		CmdKey thisKey = keys[i];
		id thisLevelsBits = thisKey; 

		// Subtract this level's first cmd's value to get a relative value
		const uint cmdLevelBase = helpers::keyNumOffsets[i]; 
		thisLevelsBits -= cmdLevelBase;

		// The amount this level has to be lshifted by
		const uint amountToShift = helpers::keyLevelBitOffset(i);
		thisLevelsBits <<= amountToShift;

		// OR it to the full ID
		sequenceID |= thisLevelsBits;
	}
	return sequenceID;
}