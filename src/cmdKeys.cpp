#include "Common/Systems/Locator/cmdKeys.h"

#include "Common/Hash.h"

locatorHasher::id locatorHasher::toID(const char *key, size_t size)
{
	return CRC32::Get(key, size);
}