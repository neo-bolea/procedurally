#pragma once

#include <functional>
#include <tuple>
#include <utility>

//// FunctionTraits ////
// See https://qiita.com/angeart/items/94734d68999eca575881 for more information.
template<class R, class Class, class IsMutable_, class... Args>
struct FunctionTraitsImpl
{
	using IsMutable = IsMutable_;

	using ReturnType = R;

	static constexpr size_t ArgCount = sizeof...(Args);

	template<size_t N>
	struct Argument
	{ using type = typename std::tuple_element<N, std::tuple<Args...>>::type; };
};

template<class Lambda>
struct FunctionTraits
	: FunctionTraits<decltype(&Lambda::operator())>
{};

template<class Return, class Class, class... Args>
struct FunctionTraits<Return(Class::*)(Args...)>
	: FunctionTraitsImpl<Return, Class, std::true_type, Args...>
{};

template<class Return, class Class, class... Args>
struct FunctionTraits<Return(Class::*)(Args...) const>
	: FunctionTraitsImpl<Return, Class, std::false_type, Args...>
{};


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
	struct is_placeholder<placeholder_template<N>> : integral_constant<int, N+1> {};
}

template<class Ret, class Member, class... Args, class ToBind, size_t... Is>
std::function<Ret(Args...)> BindFirstImpl(Ret(Member::*p)(Args...), std::index_sequence<Is...>, ToBind arg)
{ return std::bind(p, arg, placeholder_template<Is>{}...); }

template<class Ret, class Member, class... Args, class ToBind>
std::function<Ret(Args...)> BindFirst(Ret(Member::*p)(Args...), ToBind arg)
{ return BindFirstImpl(p, std::make_index_sequence<sizeof...(Args)>{}, arg); }