#pragma once

#include "Vec.h"

#include <array>
#include <assert.h>
#include <type_traits>
#include <vector>
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

		template<typename TT>
		explicit Mat<T, ROWS, COLS>(const Mat<TT, ROWS, COLS> &other);

		//// Operators ////
		void operator =(const Mat<T, ROWS, COLS> &other);
		Vec<T, COLS> operator [](size_t row) const;
		Vec<T, COLS>& operator [](size_t row);

		//// Functions ////
		Mat<T, ROWS, COLS> Identity();

		//// Data ////
		union
		{
			std::array<T, ROWS * COLS> e{};
			std::array<Vec<T, COLS>, ROWS> vecs;
		};
	};

	//// Typedefs ////
	template<size_t ROWS, size_t COLS>
	using fMat = Mat<real32, ROWS, COLS>;
}