//// Constructors ////
template<typename T, size_t ROWS, size_t COLS>
Mat<T, ROWS, COLS>::Mat() 
{ for(size_t i = 0; i < ROWS * COLS; i++) { e[i] = T(); } }

template<typename T, size_t ROWS, size_t COLS>
Mat<T, ROWS, COLS>::Mat(T n) 
{ for(size_t i = 0; i < ROWS * COLS; i++) { e[i] = n; } }

template<typename T, size_t ROWS, size_t COLS>
Mat<T, ROWS, COLS>::Mat(const T *d)
{ for(size_t i = 0; i < ROWS * COLS; i++) { e[i] = d[i]; } }

template<typename T, size_t ROWS, size_t COLS>
template<class... Args>
Mat<T, ROWS, COLS>::Mat(Args... args)
{
	static_assert(sizeof...(Args) == ROWS * COLS, 
						"Invalid number of arguments for matrix.");

	std::initializer_list<T> initList{ static_cast<T>(args)... };
	std::vector<T> elems = { initList };
	for(size_t i = 0; i < ROWS * COLS; i++) 
	{ 
		e[i] = elems[i]; 
	}
}

template<typename T, size_t ROWS, size_t COLS>
Mat<T, ROWS, COLS>::Mat<T, ROWS, COLS>(const Mat<T, ROWS, COLS> &other)
{ for(size_t i = 0; i < other.e.size(); i++) { e[i] = other.e[i]; } }

template<typename T, size_t ROWS, size_t COLS>
template<typename TT>
Mat<T, ROWS, COLS>::Mat(const Mat<TT, ROWS, COLS> &other)
{
	for(size_t i = 0; i < other.e.size(); i++)
	{ e[i] = static_cast<T>(other.e[i]); }
}


//// Non-arithmetic Operators ////
template<typename T, size_t ROWS, size_t COLS>
void Mat<T, ROWS, COLS>::operator =(const Mat<T, ROWS, COLS> &other)
{ for(size_t i = 0; i < ROWS * COLS; i++) { e[i] = other[i]; } }

template<typename T, size_t ROWS, size_t COLS>
bool operator ==(const Mat<T, ROWS, COLS> &lhs, const Mat<T, ROWS, COLS> &rhs)
{ 
	bool equal = true;
	for(size_t i = 0; i < ROWS * COLS; i++) 
	{ if(lhs.e[i] != rhs.e[i]) { return false; } } 

	return true;
}

template<typename T, size_t ROWS, size_t COLS>
bool operator !=(const Mat<T, ROWS, COLS> &lhs, const Mat<T, ROWS, COLS> &rhs)
{ return !(lhs == rhs); }

template<typename T, size_t ROWS, size_t COLS>
Vec<T, COLS> Mat<T, ROWS, COLS>::operator [](size_t row) const
{ 
	assert(row < ROWS); 
	return vecs[row]; 
}

template<typename T, size_t ROWS, size_t COLS>
Vec<T, COLS>& Mat<T, ROWS, COLS>::operator [](size_t row) 
{ 
	assert(row < ROWS); 
	return vecs[row]; 
}

template<typename T, size_t ROWS, size_t COLS>
Mat<T, ROWS, COLS> Mat<T, ROWS, COLS>::operator *(float value)
{
	Mat<T, ROWS, COLS> result(*this);
	//for(size_t i = 0; i < result.v.size(); i++) { result.v[i] *= value; }
	return result;
}



//// Arithmetic Operators ////
template<typename T, size_t N, size_t M, size_t O, size_t P>
void operator *(const Mat<T, N, M> &lhs, const Mat<T, O, P> &rhs)
{ static_assert(N != M, "Matrix multiplication is only defined for \
									two matrices, where a.cols == b.rows"); }

template<typename T, size_t N, size_t M, size_t O>
Mat<T, N, O> operator *(const Mat<T, N, M> &lhs, const Mat<T, M, O> &rhs)
{
	static_assert(std::is_default_constructible_v<T>, 
						"The matrix type must be default constructible \
							for matrix multiplication to be used.");

	Mat<T, N, O> result;
	for(size_t i = 0; i < N; i++)
	{
		for(size_t j = 0; j < O; j++)
		{
			T sum = T(0.f);
			for(size_t k = 0; k < M; k++)
			{
				auto v1 = lhs[i];
				T first = v1[k];
				T sec = rhs[k][j];
				sum += first * sec;
			}
			result[i][j] = sum;
		}
	}
	return result;
}


//// Functions ////
template<typename T, size_t ROWS, size_t COLS>
Mat<T, ROWS, COLS> Mat<T, ROWS, COLS>::Identity()
{
	static_assert(std::is_arithmetic_v<T>, 
						"The identity matrix is only defined for arithmetic types.");

	Mat<T, ROWS, COLS> result;
	for(size_t i = 0; i < std::min(ROWS, COLS); i++) { result[i][i] = T(1.f); }
	return result;
}