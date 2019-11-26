#pragma once

#include "Vec.h"

#include <array>
#include <assert.h>
#include <vector>
#include <type_traits>
#include <xmmintrin.h>

namespace Math
{
	template<typename T, size_t ROWS, size_t COLS>
	struct Mat
	{
		//// Constants ////
		static const size_t Rows = ROWS, Cols = COLS;

		//// Constructors ////
		Mat(), Mat(T n), Mat(const T *d);

		template<class... Args>
		explicit Mat(Args... args);

		Mat<T, ROWS, COLS>(const Mat<T, ROWS, COLS> &other);

		template<typename TT>
		explicit Mat<T, ROWS, COLS>(const Mat<TT, ROWS, COLS> &other);

		//// Operators ////
		void operator =(const Mat<T, ROWS, COLS> &other);
		Vec<T, COLS> operator [](size_t row) const;
		Vec<T, COLS>& operator [](size_t row);

		// Arithmetic
	#define REQUIRES(...) typename = typename std::enable_if_t<__VA_ARGS__>

		Mat<T, ROWS, COLS> operator *(float value);

		//// Functions ////
		Mat<T, ROWS, COLS> Identity();

		//// Data ////
		union
		{
			std::array<T, ROWS * COLS> e{};
			std::array<T, ROWS * COLS> v;
			std::array<Vec<T, COLS>, ROWS> vecs;
		};
	};

	//// Non-arithmetic Operators ////
	template<typename T, size_t ROWS, size_t COLS>
	void Mat<T, ROWS, COLS>::operator =(const Mat<T, ROWS, COLS> &other);

	template<typename T, size_t ROWS, size_t COLS>
	bool operator ==(const Mat<T, ROWS, COLS> &lhs, const Mat<T, ROWS, COLS> &rhs);

	template<typename T, size_t ROWS, size_t COLS>
	bool operator !=(const Mat<T, ROWS, COLS> &lhs, const Mat<T, ROWS, COLS> &rhs);


	//// Arithmetic Operators ////
	template<typename T, size_t N, size_t M, size_t O, size_t P>
	void operator *(const Mat<T, N, M> &lhs, const Mat<T, O, P> &rhs);

	template<typename T, size_t N, size_t M, size_t O>
	Mat<T, N, O> operator *(const Mat<T, N, M> &lhs, const Mat<T, M, O> &rhs);


	//// Typedefs ////
	template<size_t ROWS, size_t COLS>
	using fMat = Mat<real32, ROWS, COLS>;

#include "Mat.inc"
}

template<typename T>
using Array2D = Math::Mat<T, 4, 4>;