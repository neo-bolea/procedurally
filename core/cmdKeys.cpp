#include "Systems\cmdKeys.h"

#include "Hash.h"

locatorHasher::id locatorHasher::toID(const char *key, size_t size)
{
	return CRC32::Get(key, size);
}