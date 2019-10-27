#pragma once

#include <functional>
#include <tuple>
#include <utility>

//// FunctionTraits ////

//See https://functionalcpp.wordpress.com/2013/08/05/function-traits/ for more infomation.
namespace
{
	template<class F>
	struct FunctionTraits;
	
	template<class R, class... Args>
	struct FunctionTraits<R(*)(Args...)> : public FunctionTraits<R(Args...)> {};
	
	template<class R, class... Args>
	struct FunctionTraits<R(Args...)>
	{
		using ReturnType = R;
	
		static constexpr size_t ArgCount = sizeof...(Args);
	
		template<size_t ARG_I>
		struct Argument
		{
			static_assert(ARG_I < ArgCount, "Invalid amount of arguments.");
			using type = typename std::tuple_element<ARG_I, std::tuple<Args...>>::type;
		};
	};
}

//// Template Helpers ////
template <typename Type, typename... Args> constexpr bool Contains() 
{ return (std::is_same_v<Type, Args> || ...); };

template <typename... Args> constexpr bool ContainsRef() 
{ return (std::is_reference_v<Args> || ...); };

template <typename... Args> constexpr bool ContainsPtr() 
{ return (std::is_pointer_v<Args> || ...); };


//// Placeholder Helpers ////
template<int>
struct placeholder_template {};

namespace std
{
	template<int N>
	struct is_placeholder<placeholder_template<N>> 
		: integral_constant<int, N+1> {};
}

template<class Ret, class Member, class... Args, class ToBind, size_t... Is>
std::function<Ret(Args...)> BindFirstImpl(Ret(Member::*p)(Args...), std::index_sequence<Is...>, ToBind arg)
{
	return std::bind(p, arg, placeholder_template<Is>{}...);
}

template<class Ret, class Member, class... Args, class ToBind>
std::function<Ret(Args...)> BindFirst(Ret(Member::*p)(Args...), ToBind arg)
{
	return BindFirstImpl(p, std::make_index_sequence<sizeof...(Args)>{}, arg);
}