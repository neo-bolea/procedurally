#pragma once

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
struct VecData<T, N, std::enable_if_t<N == 1>>
{
	union
	{
		std::array<T, N> e{};
		struct { T x; };
	};
};

template<typename T, size_t N>
struct VecData<T, N, std::enable_if_t<N == 2>>
{
	union
	{
		std::array<T, N> e{};
		struct { T x, y; };
	};
};

template<typename T, size_t N>
struct VecData<T, N, std::enable_if_t<N == 3>>
{
	union
	{
		std::array<T, N> e{};
		struct { T x, y, z; };
	};
};

template<typename T, size_t N>
struct VecData<T, N, std::enable_if_t<N == 4>>
{
	union
	{
		std::array<T, N> e{};
		struct { T x, y, z, w; };
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
	static const size_t Count = N;

	//// Constructors ////
	Vec(), Vec(T n), Vec(const T *d);

	Vec<T, N>(const Vec<T, N> &other);

	template<class... Args>
	explicit Vec(Args... args);

	template<size_t NN>
	explicit Vec<T, N>(const Vec<T, NN> &other);

	template<typename TT>
	explicit Vec<T, N>(const Vec<TT, N> &other);

	template<typename TT, size_t NN>
	explicit Vec<T, N>(const Vec<TT, NN> &other);

	//// Operators ////
	void operator =(const Vec<T, N> &other);
	T operator [](size_t i) const;
	T& operator [](size_t i);

	// __m128 conversions
	template<typename = typename std::enable_if_t<Vec<T, N>::SIMDReady>>
	operator __m128() const;
	template<typename = typename std::enable_if_t<Vec<T, N>::SIMDReady>>
	void operator =(const __m128 &n);

	//// Functions ////
	std::string ToString() const;

	T Len() const;
	T LenSqr() const;
	static T Dot(const Vec<T, N> &a, const Vec<T, N> &b);
	static Vec<T, N> Cross(const Vec<T, N> &a, const Vec<T, N> &b);
	Vec<T, N> Normalize() const;
};


//// Non-arithmetic operators ////
template<typename T, size_t N>
bool operator ==(const Vec<T, N> &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N>
bool operator !=(const Vec<T, N> &lhs, const Vec<T, N> &rhs);


//// Arithmetic operators ////
template<typename T, size_t N>
Vec<T, N> operator +(const Vec<T, N> &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N>
Vec<T, N> operator -(const Vec<T, N> &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N>
Vec<T, N> operator *(const Vec<T, N> &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N, typename U>
Vec<T, N> operator *(const Vec<T, N> &lhs, const U &rhs);

template<typename T, size_t N, typename U>
Vec<T, N> operator *(const U &lhs, const Vec<T, N> &rhs);

template<typename T, size_t N, typename U>
Vec<T, N> operator /(const Vec<T, N> &lhs, const U &rhs);


//// Typedefs ////
template<typename T = float>
using Vec2 = Vec<T, 2>;
template<typename T = float>
using Vec3 = Vec<T, 3>;
template<typename T = float>
using Vec4 = Vec<T, 4>;

using fVec2 = Vec2<float>;
using fVec3 = Vec3<float>;
using fVec4 = Vec4<float>;

using dVec2 = Vec2<double>;
using dVec3 = Vec3<double>;
using dVec4 = Vec4<double>;

using iVec2 = Vec2<int>;
using iVec3 = Vec3<int>;
using iVec4 = Vec4<int>;

using uVec2 = Vec2<size_t>;
using uVec3 = Vec3<size_t>;
using uVec4 = Vec4<size_t>;

using bVec2 = Vec2<bool>;
using bVec3 = Vec3<bool>;
using bVec4 = Vec4<bool>;

using Vector2 = fVec2;
using Vector3 = fVec3;
using Vector4 = fVec4;

#include "Vec.inc"