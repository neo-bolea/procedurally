#pragma once

#include "Common/Preprocessor.h"
#include "Common/Templates.h"
#include "math/MathExt.h"

#include <array>
#include <numeric>
#include <sstream>
#include <type_traits>
#include <vector>
#include <xmmintrin.h>

//// Vector XYZW ////
// Enables the use of .x, .y, .z, .w, only for correctly sized vectors.

template<typename T, size_t N, typename Enable = void>
struct VecData;

template<typename T, size_t N>
struct Vec;

template<typename T, size_t N>
struct VecData<T, N, std::enable_if_t<N == 1>>
{
	union
	{
		struct { T x; };
		std::array<T, N> e{};
	};
};

template<typename T, size_t N>
struct VecData<T, N, std::enable_if_t<N == 2>>
{
	union
	{
		struct { T x, y; };
		std::array<T, N> e{};
	};

	static const Vec<T, N> Up, Down, Left, Right,
		Zero, One, OnlyX, NoX, NegX, OnlyY, NoY, NegY;
};

template<typename T, size_t N>
struct VecData<T, N, std::enable_if_t<N == 3>>
{
	union
	{
		struct { T x, y, z; };
		std::array<T, N> e{};
	};

	static const Vec<T, N> Up, Down, Left, Right, Forward, Backward,
		Zero, One, OnlyX, NoX, NegX, OnlyY, NoY, NegY, OnlyZ, NoZ, NegZ;
};

template<typename T, size_t N>
struct VecData<T, N, std::enable_if_t<N == 4>>
{
	union
	{
		struct { T x, y, z, w; };
		std::array<T, N> e{};
	};
};

template<typename T, size_t N>
struct VecData<T, N, std::enable_if_t<(N >= 5)>>
{
	std::array<T, N> e{};
};
#pragma endregion

template<typename T, size_t N>
struct Vec : public VecData<T, N>
{
	//// Constants ////
	static const bool SIMDReady = (N == 4) && std::is_arithmetic_v<T>;
	using Type = T;
	static const size_t Count = N;

	//// Constructors ////
	constexpr Vec();

	template<typename U>
	constexpr Vec(U n);	
	template<typename U>
	constexpr Vec(const U *d);

	constexpr Vec<T, N>(const VecData<T, N> &other);
	constexpr Vec<T, N>(const Vec<T, N> &other);

	template<size_t NN>
	explicit constexpr Vec<T, N>(const Vec<T, NN> &other);

	template<typename TT>
	explicit constexpr Vec<T, N>(const Vec<TT, N> &other);

	template<typename TT, size_t NN>
	explicit constexpr Vec<T, N>(const Vec<TT, NN> &other);

	template<class... Args>
	explicit constexpr Vec(Args... args);

	constexpr operator VecData<T, N>();

	//// Operators ////
	constexpr Vec<T, N> operator -();
	constexpr void operator =(const Vec<T, N> &other);
	constexpr T operator [](size_t i) const;
	constexpr T& operator [](size_t i);

	// __m128 conversions
	template<typename = typename std::enable_if_t<Vec<T, N>::SIMDReady>>
	constexpr operator __m128() const;
	template<typename = typename std::enable_if_t<Vec<T, N>::SIMDReady>>
	constexpr void operator =(const __m128 &n);

	//// Functions ////
	std::string Vec<T, N>::ToString() const;
	constexpr T Len() const;
	constexpr T LenSqr() const;
	static constexpr T Dot(const Vec<T, N> &a, const Vec<T, N> &b);
	static constexpr Vec<T, N> Cross(const Vec<T, N> &a, const Vec<T, N> &b);
	static constexpr Vec<T, N> Lerp(const Vec<T, N> &a, const Vec<T, N>& b, T t);
	constexpr Vec<T, N> Normalize() const;

	static constexpr Vec<T, N> Clamp(const Vec<T, N> &v,
		const Vec<T, N> &min, const Vec<T, N> &max);
	constexpr bool InRange(const Vec<T, N> &min, const Vec<T, N> &max) const;

	template<REQUIRES(N == 2)>
	constexpr T ToDegree();
	template<REQUIRES(N == 2)>
	constexpr Vec<T, N> Rotate(Vec<T, N> v, T d);
	template<REQUIRES(N == 2)>
	constexpr Vec<T, N> FromDegree(float deg);

	void ForAll(function_view<void(size_t)> func) const;
};


//// Non-arithmetic operators ////
template<typename T, size_t N>
constexpr bool operator ==(const Vec<T, N> &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N>
constexpr bool operator !=(const Vec<T, N> &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N>
std::ostream &operator<<(std::ostream &os, const Vec<T, N> &v);

template<typename T, size_t N>
std::istream &operator>>(std::istream &is, Vec<T, N> &v);


//// Arithmetic operators ////
template<typename T, size_t N>
constexpr Vec<T, N> operator +(const Vec<T, N> &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N>
constexpr Vec<T, N> operator -(const Vec<T, N> &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N>
constexpr Vec<T, N> operator *(const Vec<T, N> &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N, typename U, REQUIRES(std::is_arithmetic_v<U>)>
constexpr auto operator *(const Vec<T, N> &lhs, const U &rhs);

template<typename T, size_t N, typename U, REQUIRES(std::is_arithmetic_v<U>)>
constexpr auto operator *(const U &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N, typename U, REQUIRES(std::is_arithmetic_v<U>)>
constexpr auto operator /(const Vec<T, N> &lhs, const U &rhs);


template<typename T, size_t N>
constexpr Vec<T, N> &operator +=(const Vec<T, N> &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N>
constexpr Vec<T, N> &operator -=(const Vec<T, N> &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N>
constexpr Vec<T, N> &operator *=(const Vec<T, N> &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N, typename U, REQUIRES(std::is_arithmetic_v<U>)>
constexpr Vec<T, N> &operator *=(const Vec<T, N> &lhs, const U &rhs);

template<typename T, size_t N, typename U, REQUIRES(std::is_arithmetic_v<U>)>
constexpr Vec<T, N> &operator /=(const Vec<T, N> &lhs, const U &rhs);


//// Typedefs ////
template<typename T = float>
using Vec2 = Vec<T, 2>;
template<typename T = float>
using Vec3 = Vec<T, 3>;
template<typename T = float>
using Vec4 = Vec<T, 4>;

template<size_t N>
using fVec = Vec<float, N>;
using fVec2 = Vec2<float>;
using fVec3 = Vec3<float>;
using fVec4 = Vec4<float>;

template<size_t N>
using dVec = Vec<double, N>;
using dVec2 = Vec2<double>;
using dVec3 = Vec3<double>;
using dVec4 = Vec4<double>;

template<size_t N>
using iVec = Vec<int, N>;
using iVec2 = Vec2<int>;
using iVec3 = Vec3<int>;
using iVec4 = Vec4<int>;

template<size_t N>
using uVec = Vec<size_t, N>;
using uVec2 = Vec2<uint>;
using uVec3 = Vec3<uint>;
using uVec4 = Vec4<uint>;

template<size_t N>
using bVec = Vec<bool, N>;
using bVec2 = Vec2<bool>;
using bVec3 = Vec3<bool>;
using bVec4 = Vec4<bool>;

using Vector2 = fVec2;
using Vector3 = fVec3;
using Vector4 = fVec4;

#include "Common/Vec.inc"