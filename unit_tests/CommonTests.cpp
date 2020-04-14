#include "CommonTests.h"

#include "Common/Hash.h"
#include "Common/StringUtils.h"

#include "catch.hpp"

TEST_CASE("CRC32 Hash", "[common][hash][crc]")
{
	std::string s1 = "TestHash";
	std::string s2 = "Test";
	s2 += "Hash";
	std::string s3 = "TestHash_";
	const char ca1[9] = {'T','e','s','t','H','a','s','h'};
	const char ca2[8] = {'A','n','o','t','h','e','r'};

	SECTION("Compile time and runtime CRC32 have consistent results")
	{
		CHECK(CRC32::Get(ca1, 9) == CRC32::GetLiteral("TestHash"));
		CHECK(CRC32::Get(ca2, 8) == CRC32::GetLiteral("Another"));
	}
}