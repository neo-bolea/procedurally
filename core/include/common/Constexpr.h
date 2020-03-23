#pragma once

namespace Constexpr
{
	constexpr size_t strlen(const char *str) 
	{ return *str ? 1 + strlen(str + 1) : 0; }

	template <class T>
	constexpr void isConstexprHelper(T&& t) {}
}

#define IS_CONSTEXPR(...) noexcept(Constexpr::isConstexprHelper(__VA_ARGS__))