#pragma once

#include "Vec.h"

#include <array>
#include <assert.h>
#include <vector>
#include <type_traits>
#include <xmmintrin.h>
#define COLUMN_MAJOR

#if !defined ROW_MAJOR && !defined COLUMN_MAJOR
#define ROW_MAJOR
#endif

#if defined ROW_MAJOR && defined COLUMN_MAJOR
#error Only row-major or column-major matrices should be enabled!
#endif

namespace Math
{
	template<typename T, size_t ROWS, size_t COLS>
	struct Mat
	{
		struct MatProxy
		{
			constexpr const T &operator [](size_t i) const;
			constexpr T &operator [](size_t i);

		private:
			friend Mat;
			constexpr MatProxy(T *ptr);

			T *ptr;
		};

		//// Constants ////
		static constexpr size_t Rows = ROWS, Cols = COLS;
		static constexpr bool SIMD_READY
			= (ROWS == 4) && (COLS == 4) && std::is_arithmetic_v<T>;

		//// Constructors ////
		constexpr Mat(), Mat(T n), Mat(const T *d);

		template<class... Args>
		explicit constexpr Mat(Args... args);

		constexpr Mat<T, ROWS, COLS>(const Mat<T, ROWS, COLS> &other);

		template<typename TT>
		explicit constexpr Mat<T, ROWS, COLS>(const Mat<TT, ROWS, COLS> &other);

		//// Operators ////
		constexpr void operator =(const Mat<T, ROWS, COLS> &other);
		constexpr const MatProxy operator [](size_t row) const;
		constexpr MatProxy operator [](size_t row);

		//// Functions ////
		constexpr Mat<T, ROWS, COLS> Identity();
		constexpr Mat<T, COLS, ROWS> Transpose();

		//// Data ////
		union
		{
			std::array<Vec<T, COLS>, ROWS> vecs;
			std::array<T, ROWS * COLS> e{};
			std::array<T, ROWS * COLS> v;
		};
	};

	//// Non-arithmetic Operators ////
	template<typename T, size_t ROWS, size_t COLS>
	constexpr void Mat<T, ROWS, COLS>::operator =(const Mat<T, ROWS, COLS> &other);

	template<typename T, size_t ROWS, size_t COLS>
	constexpr bool operator ==(const Mat<T, ROWS, COLS> &lhs, const Mat<T, ROWS, COLS> &rhs);

	template<typename T, size_t ROWS, size_t COLS>
	constexpr bool operator !=(const Mat<T, ROWS, COLS> &lhs, const Mat<T, ROWS, COLS> &rhs);


	//// Arithmetic Operators ////
	template<typename T, size_t N, size_t M, size_t O, size_t P>
	constexpr void operator *(const Mat<T, N, M> &lhs, const Mat<T, O, P> &rhs);

	template<typename T, size_t N, size_t M, size_t O>
	constexpr Mat<T, N, O> operator *(const Mat<T, N, M> &lhs, const Mat<T, M, O> &rhs);

	template<typename T, size_t N, size_t M, size_t O>
	constexpr Vec<T, N> operator *(const Mat<T, N, M> &lhs, const Vec<T, M> &rhs);

	template<typename T, size_t ROWS, size_t COLS>
	constexpr Mat<T, ROWS, COLS> operator *(Mat<T, ROWS, COLS> &lhs, T rhs);

	template<typename T, size_t ROWS, size_t COLS>
	constexpr Mat<T, ROWS, COLS> operator *(T lhs, Mat<T, ROWS, COLS> &rhs);

	//// Typedefs ////
	template<size_t ROWS, size_t COLS>
	using fMat = Mat<real32, ROWS, COLS>;
	using fMat3x3 = Mat<real32, 3, 3>;
	using fMat4 = Mat<real32, 4, 4>;

	template<size_t ROWS, size_t COLS>
	using dMat = Mat<real64, ROWS, COLS>;
	using dMat3x3 = Mat<real64, 3, 3>;
	using dMat4 = Mat<real64, 4, 4>;

	using Mat3x3 = fMat3x3;
	using Mat4 = fMat4;

#include "Mat.inc"
}