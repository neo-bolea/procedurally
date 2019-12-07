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
	auto Clamp(T n, T min, T max) -> decltype(n)
	{
		n = (n < min) ? min : n;
		n = (n > max) ? max : n;
		return n;
	}

	template<typename T, REQUIRES(std::is_arithmetic_v<T>)>
	auto Lerp(T start, T end, T t) -> decltype(t)
	{
		return start * (static_cast<T>(1) - t) + end * t;
	}
}