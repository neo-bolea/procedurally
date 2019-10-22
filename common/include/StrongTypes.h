/*#pragma once

#include <type_traits>

template <class Tag, typename T>
class StrongTypedef
{
public:
	StrongTypedef() : value() {}
	explicit StrongTypedef(const T& value) : value(value) {}
	explicit StrongTypedef(T&& value) : value(std::move(value)) {}

	explicit operator T&() { return value; }
	explicit operator const T&() const { return value; }

	friend void swap(StrongTypedef &a, StrongTypedef &b) 
	{ std::swap(static_cast<T&>(a), static_cast<T&>(b)); }

private:
	T value;
};

template <typename Tag, typename T>
T underlyingTypeImpl(StrongTypedef<Tag, T>);

template <typename T>
using underlyingType = decltype(underlyingTypeImpl(std::declval<T>()));

template <class StrongTypedef>																					
struct addition																										
{																															
	friend StrongTypedef &operator +=(StrongTypedef &lhs, const StrongTypedef &rhs)				
	{																														
		using type = underlyingType<StrongTypedef>;															
		static_cast<type &>(lhs) += static_cast<const type &>(rhs);										
		return lhs;																										
	}																														
																															
	friend StrongTypedef operator +(const StrongTypedef &lhs, const StrongTypedef &rhs)			
	{																														
		using type = underlyingType<StrongTypedef>;															
		return StrongTypedef(static_cast<const type &>(lhs) +  static_cast<const type &>(rhs));
	}																														
};

template <class StrongTypedef>																					
struct subtraction																										
{																															
	friend StrongTypedef &operator -=(StrongTypedef &lhs, const StrongTypedef &rhs)				
	{																														
		using type = underlyingType<StrongTypedef>;															
		static_cast<type &>(lhs) -= static_cast<const type &>(rhs);										
		return lhs;																										
	}																														

	friend StrongTypedef operator -(const StrongTypedef &lhs, const StrongTypedef &rhs)			
	{																														
		using type = underlyingType<StrongTypedef>;															
		return StrongTypedef(static_cast<const type &>(lhs) -  static_cast<const type &>(rhs));
	}																														
};

template <class StrongTypedef>																					
struct multiplication																										
{																															
	friend StrongTypedef &operator *=(StrongTypedef &lhs, const StrongTypedef &rhs)				
	{																														
		using type = underlyingType<StrongTypedef>;															
		static_cast<type &>(lhs) *= static_cast<const type &>(rhs);										
		return lhs;																										
	}																														

	friend StrongTypedef operator *(const StrongTypedef &lhs, const StrongTypedef &rhs)			
	{																														
		using type = underlyingType<StrongTypedef>;															
		return StrongTypedef(static_cast<const type &>(lhs) *  static_cast<const type &>(rhs));
	}																														
};

template <class StrongTypedef>																					
struct division																										
{																															
	friend StrongTypedef &operator /=(StrongTypedef &lhs, const StrongTypedef &rhs)				
	{																														
		using type = underlyingType<StrongTypedef>;															
		static_cast<type &>(lhs) /= static_cast<const type &>(rhs);										
		return lhs;																										
	}																														

	friend StrongTypedef operator /(const StrongTypedef &lhs, const StrongTypedef &rhs)			
	{																														
		using type = underlyingType<StrongTypedef>;															
		return StrongTypedef(static_cast<const type &>(lhs) /  static_cast<const type &>(rhs));
	}																														
};

template <class StrongTypedef>																					
struct modulo																										
{																															
	friend StrongTypedef &operator %=(StrongTypedef &lhs, const StrongTypedef &rhs)				
	{																														
		using type = underlyingType<StrongTypedef>;															
		static_cast<type &>(lhs) %= static_cast<const type &>(rhs);										
		return lhs;																										
	}																														

	friend StrongTypedef operator %(const StrongTypedef &lhs, const StrongTypedef &rhs)			
	{																														
		using type = underlyingType<StrongTypedef>;															
		return StrongTypedef(static_cast<const type &>(lhs) %  static_cast<const type &>(rhs));
	}																														
};

template <class StrongTypedef>																					
struct increment																										
{		
	friend StrongTypedef &operator ++(StrongTypedef &e)				
	{																														
		using type = underlyingType<StrongTypedef>;															
		static_cast<const type &>(++e)
		return StrongTypedef(e);
	}	

	friend StrongTypedef &operator ++(StrongTypedef &e, int)				
	{																														
		using type = underlyingType<StrongTypedef>;															
		static_cast<type &> tmp(e);
		e.operator++();
		return StrongTypedef(tmp);
	}																												
};

template <class StrongTypedef>																					
struct decrement																										
{		
	friend StrongTypedef &operator --(StrongTypedef &e)				
	{																														
		using type = underlyingType<StrongTypedef>;															
		static_cast<const type &>(--e)
			return StrongTypedef(e);
	}	

	friend StrongTypedef &operator --(StrongTypedef &e, int)				
	{																														
		using type = underlyingType<StrongTypedef>;															
		static_cast<type &> tmp(e);
		e.operator--();
		return StrongTypedef(tmp);
	}																												
};

template <class StrongTypedef>																					
struct unaryPlus																										
{																															
	friend StrongTypedef &operator +(StrongTypedef &e)				
	{																														
		using type = underlyingType<StrongTypedef>;															
		static_cast<type &>(e) = +static_cast<const type &>(e);										
		return StrongTypedef(e);																										
	}																														
};

template <class StrongTypedef>																					
struct unaryMinus																										
{																															
	friend StrongTypedef &operator -(StrongTypedef &e)				
	{																														
		using type = underlyingType<StrongTypedef>;															
		static_cast<type &>(e) = -static_cast<const type &>(e);										
		return StrongTypedef(e);																										
	}																														
};

template <class StrongTypedef, class OtherType>																					
struct mixedAddition																										
{													
	friend StrongTypedef &operator +=(StrongTypedef &lhs, const OtherType &rhs)				
	{ return static_cast<underlyingType<StrongTypedef> &>(lhs) += rhs; }																														

	friend StrongTypedef operator +(const StrongTypedef &lhs, const OtherType &rhs)			
	{ return StrongTypedef(static_cast<const underlyingType<StrongTypedef> &>(lhs) + rhs); }	

	friend StrongTypedef operator +(const OtherType &lhs, const StrongTypedef &rhs)			
	{ return StrongTypedef(lhs + static_cast<const underlyingType<StrongTypedef> &>(rhs)); }	
};

template <class StrongTypedef, class OtherType>																					
struct mixedSubtraction																										
{																															
	friend StrongTypedef &operator -=(StrongTypedef &lhs, const OtherType &rhs)				
	{ return static_cast<underlyingType<StrongTypedef> &>(lhs) -= rhs; }																														

	friend StrongTypedef operator -(const StrongTypedef &lhs, const OtherType &rhs)			
	{
		return StrongTypedef(	 - rhs); 
	}	

	friend StrongTypedef operator -(const OtherType &lhs, const StrongTypedef &rhs)			
	{ return StrongTypedef(lhs - static_cast<const underlyingType<StrongTypedef> &>(rhs)); }																														
};

template <class StrongTypedef, class OtherType>																					
struct mixedMultiplication																										
{																															
	friend StrongTypedef &operator *=(StrongTypedef &lhs, const OtherType &rhs)				
	{ return static_cast<underlyingType<StrongTypedef> &>(lhs) *= rhs; }																														

	friend StrongTypedef operator *(const StrongTypedef &lhs, const OtherType &rhs)			
	{ return StrongTypedef(static_cast<const underlyingType<StrongTypedef> &>(lhs) * rhs); }			
	
	friend StrongTypedef operator *(const OtherType &lhs, const StrongTypedef &rhs)			
	{ return StrongTypedef(lhs * static_cast<const underlyingType<StrongTypedef> &>(rhs)); }																														
};

template <class StrongTypedef, class OtherType>																					
struct mixedDivision																										
{																															
	friend StrongTypedef &operator /=(StrongTypedef &lhs, const OtherType &rhs)				
	{ return static_cast<underlyingType<StrongTypedef> &>(lhs) /= rhs; }																														

	friend StrongTypedef operator /(const StrongTypedef &lhs, const OtherType &rhs)			
	{ return StrongTypedef(static_cast<const underlyingType<StrongTypedef> &>(lhs) / rhs); }	

	friend StrongTypedef operator /(const OtherType &lhs, const StrongTypedef &rhs)			
	{ return StrongTypedef(lhs /  static_cast<const underlyingType<StrongTypedef> &>(rhs)); }																														
};

template <class StrongTypedef, class OtherType>																					
struct mixedModulo																										
{																															
	friend StrongTypedef &operator %=(StrongTypedef &lhs, const OtherType &rhs)				
	{ return static_cast<underlyingType<StrongTypedef> &>(lhs) %= rhs; }																														

	friend StrongTypedef operator %(const StrongTypedef &lhs, const OtherType &rhs)			
	{ return StrongTypedef(static_cast<const underlyingType<StrongTypedef> &>(lhs) % rhs); }	

	friend StrongTypedef operator %(const OtherType &lhs, const StrongTypedef &rhs)			
	{ return StrongTypedef(lhs %  static_cast<const underlyingType<StrongTypedef> &>(rhs)); }																														
};

template<class StrongTypedef>
struct realArithmetic : 
	unaryPlus<StrongTypedef>,
	unaryMinus<StrongTypedef>,
	addition<StrongTypedef>,
	subtraction<StrongTypedef>,
	multiplication<StrongTypedef>,
	division<StrongTypedef>,
	increment<StrongTypedef>,
	decrement<StrongTypedef>
{};

template<class StrongTypedef>
struct integerArithmetic : 
	unaryPlus<StrongTypedef>,
	unaryMinus<StrongTypedef>,
	addition<StrongTypedef>,
	subtraction<StrongTypedef>,
	multiplication<StrongTypedef>,
	division<StrongTypedef>,
	modulo<StrongTypedef>,
	increment<StrongTypedef>,
	decrement<StrongTypedef>
{};

template<class StrongTypedef, class OtherType>
struct mixedRealArithmetic : 
	mixedAddition<StrongTypedef, OtherType>,
	mixedSubtraction<StrongTypedef, OtherType>,
	mixedMultiplication<StrongTypedef, OtherType>,
	mixedDivision<StrongTypedef, OtherType>
{};

template<class StrongTypedef, class OtherType>
struct mixedIntegerArithmetic : 
	mixedAddition<StrongTypedef, OtherType>,
	mixedSubtraction<StrongTypedef, OtherType>,
	mixedMultiplication<StrongTypedef, OtherType>,
	mixedDivision<StrongTypedef, OtherType>,
	mixedModulo<StrongTypedef, OtherType>
{};

template<class StrongTypedef, class OtherType>
struct mathematicalArithmetic {};


template <class StrongTypedef, class OtherType>																					
struct convertible																										
{	
	using convtype = underlyingType<StrongTypedef>;
	convertible() {}
	explicit convertible(const OtherType& value) : value(static_cast<convtype>(value)) {}
	explicit convertible(OtherType&& value) : value(std::move(static_cast<convtype>(value))) {}

	explicit operator OtherType&() { return static_cast<OtherType>(value); }
	explicit operator const OtherType&() const { return static_cast<OtherType>(value); }
};

template <class StrongTypedef>																					
struct numberConvertible :
	convertible<StrongTypedef, float>,
	convertible<StrongTypedef, double>,
	convertible<StrongTypedef, long double>,

	convertible<StrongTypedef, char>,
	convertible<StrongTypedef, unsigned char>,
	convertible<StrongTypedef, short>,
	convertible<StrongTypedef, unsigned short>,
	convertible<StrongTypedef, int>,
	convertible<StrongTypedef, unsigned int>,
	convertible<StrongTypedef, long>,
	convertible<StrongTypedef, unsigned long>,
	convertible<StrongTypedef, long long>,
	convertible<StrongTypedef, unsigned long long>
{};

#define MATHEMATICAL_REAL_SPECIALIZATION(TYPE)	      \
template<class StrongTypedef>									\
struct mathematicalArithmetic<StrongTypedef, TYPE> :  \
	convertible<StrongTypedef, TYPE>,						\
	mixedRealArithmetic<StrongTypedef, TYPE>				\
{};

#define MATHEMATICAL_INTEGER_SPECIALIZATION(TYPE)	   \
template<class StrongTypedef>									\
struct mathematicalArithmetic<StrongTypedef, TYPE> :  \
	convertible<StrongTypedef, TYPE>,						\
	mixedIntegerArithmetic<StrongTypedef, TYPE>			\
{};

MATHEMATICAL_REAL_SPECIALIZATION(float);
MATHEMATICAL_REAL_SPECIALIZATION(double);
MATHEMATICAL_REAL_SPECIALIZATION(long double);

MATHEMATICAL_INTEGER_SPECIALIZATION(char);
MATHEMATICAL_INTEGER_SPECIALIZATION(unsigned char);
MATHEMATICAL_INTEGER_SPECIALIZATION(short);
MATHEMATICAL_INTEGER_SPECIALIZATION(unsigned short);
MATHEMATICAL_INTEGER_SPECIALIZATION(int);
MATHEMATICAL_INTEGER_SPECIALIZATION(unsigned int);
MATHEMATICAL_INTEGER_SPECIALIZATION(long);
MATHEMATICAL_INTEGER_SPECIALIZATION(unsigned long);
MATHEMATICAL_INTEGER_SPECIALIZATION(long long);
MATHEMATICAL_INTEGER_SPECIALIZATION(unsigned long long);

#undef MATHEMATICAL_REAL_SPECIALIZATION
#undef MATHEMATICAL_INTEGER_SPECIALIZATION*/