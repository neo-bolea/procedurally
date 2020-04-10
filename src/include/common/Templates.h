#pragma once
#include <functional>
#include <numeric>
#include <tuple>
#include <type_traits>
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


// See https://github.com/SuperV1234/vittorioromeo.info/blob/master/extra/passing_functions_to_functions/function_view.hpp for more information.
template <typename TSignature>
class function_view;

template <typename TReturn, typename... TArgs>
class function_view<TReturn(TArgs...)> final
{
private:
	using signature_type = TReturn(void *, TArgs...);

	void *_ptr;
	TReturn (*_erased_fn)(void *, TArgs...);

public:
	function_view() {}

	template<typename T> function_view(T &&x) : _ptr{ (void *)std::addressof(x) }
	{
		_erased_fn = [](void *ptr, TArgs... xs) -> TReturn {
			return (*reinterpret_cast<std::add_pointer_t<T>>(ptr))(
				std::forward<TArgs>(xs)...);
		};
	}

	decltype(auto) operator()(TArgs... xs) const
	{ return _erased_fn(_ptr, std::forward<TArgs>(xs)...); }
};


// Dummy Template for passing values without hashing.
template<typename T, typename Result = uint>
struct PassHasher
{
	Result operator()(const T &value) const
	{ return value; }
};

template<typename Hasher, typename Container,
	typename Result = decltype(Hasher())>
struct AccumulateHasher
{
	Result operator()(const Container &container) const
	{
		std::vector<Result> tmp(container.size());
		std::transform(container.begin(), container.end(), tmp.begin(), Hasher());
		return std::accumulate(tmp.begin(), tmp.end(), size_t());
	}
};