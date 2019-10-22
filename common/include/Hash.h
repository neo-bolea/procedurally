#pragma once

#include <cstring>
#include <cstdint>
#include <iostream>

namespace CRC32
{
	constexpr uint32 Polynomial = 0xEDB88320;

	// See https://create.stephan-brumme.com/crc32/ for more information.
	// Compute CRC32 (Slicing-by-8 algorithm)
	uint32 Get(const void *data, size_t length);
}

namespace CRC32
{
	template<size_t idx>
	constexpr uint32 crcinner(const uint32 crc)
	{ return (crcinner<idx-1>(crc) >> 1) ^ (-int32(crcinner<idx-1>(crc) & 1) & Polynomial); }

	template<>
	constexpr uint32 crcinner<size_t(0)>(const uint32 crc)
	{ return crc; }

	template<size_t idx>
	constexpr uint32 crcouter(const char *str)
	{ return crcinner<8>(crcouter<idx-1>(str) ^ str[idx - 1]); }

	template<>
	constexpr uint32 crcouter<size_t(0)>(const char *str)
	{ return 0xFFFFFFFF; }

	template<size_t ARG_I>
	constexpr uint32 GetConst(const char (&str)[ARG_I])
	{ return ~crcouter<ARG_I>(str); }
}

namespace MurmurHash64A
{
	const uint64 seed = 509019843274;
	constexpr uint64 Get(const void *key, const int len)
	{
		const uint64 m = 0xc6a4a7935bd1e995LLU;
		const int r = 47;

		uint64 h = seed ^ (len * m);

		const uint64 *data = (const uint64 *)key;
		const uint64 *end = (len >> 3) + data;

		while(data != end)
		{
			uint64 k = *data++;

			k *= m; 
			k ^= k >> r; 
			k *= m; 

			h ^= k;
			h *= m; 
		}

		const uchar *data2 = (uchar *)data;

		switch(len & 7)
		{
		case 7: h ^= static_cast<uint64>(static_cast<uint8>(data2[6])) << 48;
		case 6: h ^= static_cast<uint64>(data2[5]) << 40;
		case 5: h ^= static_cast<uint64>(data2[4]) << 32;
		case 4: h ^= static_cast<uint64>(data2[3]) << 24;
		case 3: h ^= static_cast<uint64>(data2[2]) << 16;
		case 2: h ^= static_cast<uint64>(data2[1]) << 8;
		case 1: h ^= static_cast<uint64>(data2[0]);
			h *= m;
		};

		h ^= h >> r;
		h *= m;
		h ^= h >> r;

		return h;
	} 
}