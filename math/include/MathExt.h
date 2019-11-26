#pragma once

#include "Preprocessor.h"

#include <type_traits>

//// Constants ////
namespace Math
{
	extern double Deg2Rad;
	extern double Rad2Deg;
}

//// Functions ////
namespace Math
{
	template<typename T, REQUIRES(std::is_arithmetic_v<T>)> 
	T Clamp(T n, T min, T max)
	{
		n = (n < min) ? min : n;
		n = (n > max) ? max : n;
		return n;
	}
}