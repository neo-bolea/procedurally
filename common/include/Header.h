#pragma once
 
// See https://foonathan.net/2016/10/strong-typedefs/ for more information.

namespace strongType
{
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

	template <class StrongTypedef>
	struct arithmetic : 
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
}