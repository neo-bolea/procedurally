#pragma once

#include "Math/MathHelpers.h"
#include "Common/Hash.h"

#include <functional>

class locatorHasher
{
private:
	friend class Locator;

	using id = uint64;

	template<size_t SIZE>
	constexpr id toID(const char *key)
	{ return CRC32::Get(key); }
	static id toID(const char *key, size_t size);
};