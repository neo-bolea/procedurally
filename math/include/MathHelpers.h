#pragma once

namespace Math
{
	constexpr uint NextPow2(uint x)
	{
		x -= 1;
		x |= (x >> 1);
		x |= (x >> 2);
		x |= (x >> 4);
		x |= (x >> 8);
		x |= (x >> 16);

		return x + 1;
	}

	static constexpr uint MSB(uint x)
	{
		uint r = 0; 
		do{ r++; } while(x >>= 1);
		return r;
	}
}