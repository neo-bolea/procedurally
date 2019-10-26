//// Constructors ////
template<typename T, size_t N>
Vec<T, N>::Vec() 
{ for(size_t i = 0; i < N; i++) { this->e[i] = 0.f; } }

template<typename T, size_t N>
Vec<T, N>::Vec(T n) 
{ for(size_t i = 0; i < N; i++) { this->e[i] = n; } }

template<typename T, size_t N>
Vec<T, N>::Vec(const T *d)
{ for(size_t i = 0; i < N; i++) { this->e[i] = d[i]; } }

template<typename T, size_t N>
template<class... Args>
Vec<T, N>::Vec(Args... args)
{
	static_assert(sizeof...(Args) == N, "Invalid number of arguments for vector.");

	std::initializer_list<T> initList{ static_cast<T>(args)... };
	std::vector<T> elems = { initList };
	for(size_t i = 0; i < N; i++) { this->e[i] = elems[i]; }
}

template<typename T, size_t N>
template<size_t NN>
Vec<T, N>::Vec(const Vec<T, NN> &other)
{ for(size_t i = 0; i < std::min(N, NN); i++) { this->e[i] = other[i]; } }

template<typename T, size_t N>
template<typename TT>
Vec<T, N>::Vec(const Vec<TT, N> &other)
{ for(size_t i = 0; i < N; i++) { this->e[i] = static_cast<T>(other[i]); } }


template<typename T, size_t N>
template<typename TT, size_t NN>
Vec<T, N>::Vec(const Vec<TT, NN> &other)
{ for(size_t i = 0; i < std::min(N, NN); i++) { this->e[i] = static_cast<T>(other[i]); } }


//// Non-arithmetic operators ////
template<typename T, size_t N>
void Vec<T, N>::operator =(const Vec<T, N> &other)
{ for(size_t i = 0; i < N; i++) { this->e[i] = other[i]; } }

template<typename T, size_t N>
T Vec<T, N>::operator [](size_t i) const 
{ return this->e[i]; }

template<typename T, size_t N>
T& Vec<T, N>::operator [](size_t i) 
{ return this->e[i]; }

template<typename T, size_t N>
bool operator ==(const Vec<T, N> &lhs, const Vec<T, N> &rhs) 
{
	for(size_t i = 0; i < N; i++)
	{ if(lhs[i] != rhs[i]) { return false; } }
	return true;
}

template<typename T, size_t N>
bool operator !=(const Vec<T, N> &lhs, const Vec<T, N> &rhs) 
{ return !(lhs == rhs); }

template<typename T, size_t N>
template<typename>
Vec<T, N>::operator __m128() const
{ return _mm_load_ps(this->e.data()); }

template<typename T, size_t N>
template<typename>
void Vec<T, N>::operator =(const __m128 &n)
{ _mm_store_ps(this->e.data(), n); }


//// Arithmetic operators ////
template<typename T, size_t N>
Vec<T, N> operator +(const Vec<T, N> &lhs, const Vec<T, N> &rhs)
{
	Vec<T, N> result;
	if constexpr(Vec<T, N>::SIMDReady)
	{ result = _mm_add_ps(lhs, rhs); }
	else
	{ for(size_t i = 0; i < N; i++) { result[i] = lhs[i] + rhs[i]; } }
	return result;
}

template<typename T, size_t N>
Vec<T, N> operator -(const Vec<T, N> &lhs, const Vec<T, N> &rhs) 
{
	Vec<T, N> result;
	if constexpr(Vec<T, N>::SIMDReady)
	{ result = _mm_sub_ps(lhs, rhs); }
	else
	{ for(size_t i = 0; i < N; i++) { result[i] = lhs[i] - rhs[i]; } }
	return result;
}

template<typename T, size_t N>
Vec<T, N> operator *(const Vec<T, N> &lhs, const Vec<T, N> &rhs) 
{
	Vec<T, N> result;
	if constexpr(Vec<T, N>::SIMDReady)
	{ result = _mm_mul_ps(lhs, rhs); }
	else
	{ for(size_t i = 0; i < N; i++) { result[i] = lhs[i] * rhs[i]; } }
	return result;
}

template<typename T, size_t N, typename U>
Vec<T, N> operator *(const Vec<T, N> &lhs, const U &rhs)
{
	Vec<T, N> result;
	if constexpr(Vec<T, N>::SIMDReady)
	{ result = _mm_mul_ps(lhs, _mm_set1_ps(rhs)); }
	else
	{ for(size_t i = 0; i < N; i++) { result[i] = lhs[i] * rhs; } }
	return result;
}

template<typename T, size_t N, typename U>
Vec<T, N> operator *(const U &lhs, const Vec<T, N> &rhs)
{
	Vec<T, N> result;
	if constexpr(Vec<T, N>::SIMDReady)
	{ result = _mm_mul_ps(rhs, _mm_set1_ps(lhs)); }
	else
	{ for(size_t i = 0; i < N; i++) { result[i] = lhs * rhs[i]; } }
	return result;
}

template<typename T, size_t N, typename U>
Vec<T, N> operator /(const Vec<T, N> &lhs, const U &rhs)
{
	Vec<T, N> result;
	if constexpr(Vec<T, N>::SIMDReady)
	{ result = _mm_div_ps(lhs, _mm_set1_ps(rhs)); }
	else
	{ for(size_t i = 0; i < N; i++) { result[i] = lhs[i] * rhs; } }
	return result;
}


//// Functions ////
template<typename T, size_t N>
std::string Vec<T, N>::ToString() const
{
	std::stringstream ss;
	ss << "(";

	for(size_t i = 0; i < N - 1; i++)
	{ ss << this->e[i] << ", "; }

	ss << this->e[N - 1] << ")";

	return ss.str();
}

template<typename T, size_t N>
T Vec<T, N>::Len() const
{ 
	static_assert(std::is_arithmetic_v<DotType>, 
					  "The square root of the given vector type is undefined.");
	return sqrt(LenSqr()); 
}

template<typename T, size_t N>
T Vec<T, N>::LenSqr() const
{ return Dot(*this, *this); }

template<typename T, size_t N>
T Vec<T, N>::Dot(const Vec<T, N> &lhs, const Vec<T, N> &rhs)
{
	static_assert(std::is_default_constructible_v<T>, 
					  "The vector type must be default constructible \
						for the dot product to be used.");

	T sum{};
	{ for(size_t i = 0; i < N; i++) { sum += lhs[i] * rhs[i]; } }

	return sum;
}