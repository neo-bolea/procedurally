//// Constants ////
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 2>>::Up{ 0, 1 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 2>>::Down{ 0, -1 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 2>>::Left{ -1, 0 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 2>>::Right{ 1, 0 };

template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 2>>::Zero{ 0, 0 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 2>>::One{ 1, 1 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 2>>::OnlyX{ 1, 0 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 2>>::NoX{ 0, 1 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 2>>::NegX{ -1, 1 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 2>>::OnlyY{ 0, 1 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 2>>::NoY{ 1, 0 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 2>>::NegY{ 1, -1 };


template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::Up{ 0, 1, 0 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::Down{ 0, -1, 0 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::Left{ -1, 0, 0 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::Right{ 1, 0, 0 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::Forward{ 0, 0, 1 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::Backward{ 0, 0, -1 };

template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::Zero{ 0, 0, 0 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::One{ 1, 1, 1 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::OnlyX{ 1, 0, 0 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::NoX{ 0, 1, 1 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::NegX{ -1, 1, 1 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::OnlyY{ 0, 1, 0 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::NoY{ 1, 0, 1 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::NegY{ 1, -1, 1 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::OnlyZ{ 0, 0, 1 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::NoZ{ 1, 1, 0 };
template<typename T, size_t N>
const Vec<T, N> VecData<T, N, std::enable_if_t<N == 3>>::NegZ{ 1, 1, -1 };


//// Constructors ////
template<typename T, size_t N>
constexpr Vec<T, N>::Vec() 
{ for(size_t i = 0; i < N; i++) { this->e[i] = static_cast<T>(0); } }

template<typename T, size_t N>
template<typename U>
constexpr Vec<T, N>::Vec(U n) 
{ 
	static_assert(std::is_arithmetic_v<U>, 
		"The given data type cannot be used as vector constructor.");
	for(size_t i = 0; i < N; i++) { this->e[i] = static_cast<T>(n); } 
}

template<typename T, size_t N>
template<typename U>
constexpr Vec<T, N>::Vec(const U *d)
{ 
	static_assert(std::is_arithmetic_v<U>, 
		"The given data type cannot be used as vector constructor.");
	for(size_t i = 0; i < N; i++) { this->e[i] = static_cast<T>(d[i]); } 
}

template<typename T, size_t N>
constexpr Vec<T, N>::Vec(const VecData<T, N> &other)
{ for(size_t i = 0; i < N; i++) { this->e[i] = other.e[i]; } }

template<typename T, size_t N>
constexpr Vec<T, N>::Vec(const Vec<T, N> &other)
{ for(size_t i = 0; i < N; i++) { this->e[i] = other[i]; } }

template<typename T, size_t N>
template<size_t NN>
constexpr Vec<T, N>::Vec(const Vec<T, NN>& other)
{
	size_t i = 0;
	for (; i < std::min(N, NN); i++) { this->e[i] = other[i]; }
	for (; i < N; i++) { this->e[i] = T(); }
}

template<typename T, size_t N>
template<typename TT>
constexpr Vec<T, N>::Vec(const Vec<TT, N> &other)
{ for(size_t i = 0; i < N; i++) { this->e[i] = static_cast<T>(other[i]); } }

template<typename T, size_t N>
template<typename TT, size_t NN>
constexpr Vec<T, N>::Vec(const Vec<TT, NN> &other)
{ 
	size_t i = 0;
	for(; i < std::min(N, NN); i++) { this->e[i] = static_cast<T>(other[i]); } 
	for(; i < N; i++) { this->e[i] = T(); } 
}

template<typename T, size_t N>
template<class... Args>
constexpr Vec<T, N>::Vec(Args... args)
{
	static_assert(sizeof...(Args) == N, "Invalid number of arguments for vector constructor.");

	std::array<T, sizeof...(Args)> elems = { static_cast<T>(args)... };
	for (size_t i = 0; i < N; i++) { this->e[i] = elems[i]; }
}


template<typename T, size_t N>
constexpr Vec<T, N>::operator VecData<T, N>()
{ return static_cast<VecData<T, N>>(*this); }


//// Non-arithmetic operators ////
template<typename T, size_t N>
constexpr void Vec<T, N>::operator =(const Vec<T, N> &other)
{ for(size_t i = 0; i < N; i++) { this->e[i] = other[i]; } }

template<typename T, size_t N>
constexpr T Vec<T, N>::operator [](size_t i) const 
{ return this->e[i]; }

template<typename T, size_t N>
constexpr T& Vec<T, N>::operator [](size_t i) 
{ return this->e[i]; }

template<typename T, size_t N>
constexpr bool operator ==(const Vec<T, N> &lhs, const Vec<T, N> &rhs) 
{
	for(size_t i = 0; i < N; i++)
	{ if(lhs[i] != rhs[i]) { return false; } }
	return true;
}

template<typename T, size_t N>
constexpr bool operator !=(const Vec<T, N> &lhs, const Vec<T, N> &rhs) 
{ return !(lhs == rhs); }

template<typename T, size_t N>
template<typename>
constexpr Vec<T, N>::operator __m128() const
{ return _mm_load_ps(this->e.data()); }

template<typename T, size_t N>
template<typename>
constexpr void Vec<T, N>::operator =(const __m128 &n)
{ _mm_store_ps(this->e.data(), n); }


//// Arithmetic operators ////
template<typename T, size_t N>
constexpr Vec<T, N> operator +(const Vec<T, N> &lhs, const Vec<T, N> &rhs)
{
	Vec<T, N> result;
	if constexpr(Vec<T, N>::SIMDReady)
	{ result = _mm_add_ps(lhs, rhs); }
	else
	{ for(size_t i = 0; i < N; i++) { result[i] = lhs[i] + rhs[i]; } }
	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> operator -(const Vec<T, N> &lhs, const Vec<T, N> &rhs) 
{
	Vec<T, N> result;
	if constexpr(Vec<T, N>::SIMDReady)
	{ result = _mm_sub_ps(lhs, rhs); }
	else
	{ for(size_t i = 0; i < N; i++) { result[i] = lhs[i] - rhs[i]; } }
	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> Vec<T, N>::operator -() { return *this * -1; }

template<typename T, size_t N>
constexpr Vec<T, N> operator *(const Vec<T, N> &lhs, const Vec<T, N> &rhs) 
{
	Vec<T, N> result;
	if constexpr(Vec<T, N>::SIMDReady)
	{ result = _mm_mul_ps(lhs, rhs); }
	else
	{ for(size_t i = 0; i < N; i++) { result[i] = lhs[i] * rhs[i]; } }
	return result;
}

template<typename T, size_t N, typename U, typename>
constexpr Vec<T, N> operator *(const Vec<T, N> &lhs, const U &rhs)
{
	Vec<T, N> result;
	if constexpr(Vec<T, N>::SIMDReady)
	{ result = _mm_mul_ps(lhs, _mm_set1_ps(rhs)); }
	else
	{ for(size_t i = 0; i < N; i++) { result[i] = static_cast<T>(lhs[i] * rhs); } }
	return result;
}

template<typename T, size_t N, typename U, typename>
constexpr Vec<T, N> operator *(const U &lhs, const Vec<T, N> &rhs)
{ return rhs * lhs; }

template<typename T, size_t N>
constexpr Vec<T, N> operator /(const Vec<T, N> &lhs, const Vec<T, N> &rhs)
{
	Vec<T, N> result;
	if constexpr(Vec<T, N>::SIMDReady)
	{ result = _mm_div_ps(lhs, rhs); }
	else
	{ for(size_t i = 0; i < N; i++) { result[i] = static_cast<T>(lhs[i] / rhs[i]); } }
	return result;
}

template<typename T, size_t N, typename U, typename>
constexpr Vec<T, N> operator /(const Vec<T, N> &lhs, const U &rhs)
{
	Vec<T, N> result;
	if constexpr(Vec<T, N>::SIMDReady)
	{ result = _mm_div_ps(lhs, _mm_set1_ps(rhs)); }
	else
	{ for(size_t i = 0; i < N; i++) { result[i] = static_cast<T>(lhs[i] / rhs); } }
	return result;
}


template<typename T, size_t N>
constexpr Vec<T, N> &operator +=(Vec<T, N> &lhs, const Vec<T, N> &rhs)
{
	lhs = lhs + rhs;
	return lhs;
}

template<typename T, size_t N>
constexpr Vec<T, N> &operator -=(Vec<T, N> &lhs, const Vec<T, N> &rhs)
{
	lhs = lhs - rhs;
	return lhs;
}

template<typename T, size_t N>
constexpr Vec<T, N> &operator *=(Vec<T, N> &lhs, const Vec<T, N> &rhs)
{
	lhs = lhs * rhs;
	return lhs;
}

template<typename T, size_t N, typename U, REQUIRES(std::is_arithmetic_v<U>)>
constexpr Vec<T, N> &operator *=(Vec<T, N> &lhs, const U &rhs)
{
	lhs = lhs * rhs;
	return lhs;
}

template<typename T, size_t N, typename U, REQUIRES(std::is_arithmetic_v<U>)>
constexpr Vec<T, N> &operator /=(Vec<T, N> &lhs, const U &rhs)
{
	lhs = lhs / rhs;
	return lhs;
}


//// Functions ////
template<typename T, size_t N>
constexpr std::string Vec<T, N>::ToString() const
{
	std::stringstream ss;
	ss << "(";

	for(size_t i = 0; i < N - 1; i++)
	{ ss << this->e[i] << ", "; }

	ss << this->e[N - 1] << ")";

	return ss.str();
}

template<typename T, size_t N>
constexpr T Vec<T, N>::Len() const
{ 
	static_assert(std::is_arithmetic_v<T>, 
					  "The square root undefined for non-arithmetic types.");
	return sqrt(LenSqr()); 
}

template<typename T, size_t N>
constexpr T Vec<T, N>::LenSqr() const
{ return Dot(*this, *this); }

template<typename T, size_t N>
constexpr T Vec<T, N>::Dot(const Vec<T, N> &lhs, const Vec<T, N> &rhs)
{
	static_assert(std::is_default_constructible_v<T>, 
					  "The vector type must be default constructible for the dot product to be used.");

	T sum{};
	for(size_t i = 0; i < N; i++) { sum += lhs[i] * rhs[i]; }

	return sum;
}

template<typename T, size_t N>
constexpr Vec<T, N> Vec<T, N>::Cross(const Vec<T, N> &a, const Vec<T, N> &b)
{
	static_assert(N == 3, "The cross product is only supported for vectors with 3 elements.");
	static_assert(std::is_arithmetic_v<T>, 
					  "The cross product is undefined for non-arithmetic types.");

	return Vec<T, N>(
		a.y * b.z - b.y * a.z,
		a.z * b.x - b.z * a.x,
		a.x * b.y - b.x * a.y);
}

template<typename T, size_t N>
constexpr Vec<T, N> Vec<T, N>::Lerp(const Vec<T, N> &a, const Vec<T, N> &b, T t)
{
	static_assert(std::is_arithmetic_v<T>,
		"Lerping is undefined for non-arithmetic types.");

	Vec<T, N> result;
	for(size_t i = 0; i < N; i++) { result[i] = Math::Lerp(a[i], b[i], t); }
	return result;
}

template<typename T, size_t N>
constexpr Vec<T, N> Vec<T, N>::Normalize() const
{ return Vec<T, N>(&e[0]) / Len(); }

template<typename T, size_t N>
constexpr Vec<T, N> Vec<T, N>::Clamp(const Vec<T, N> &v,
	const Vec<T, N> &min, const Vec<T, N> &max)
{
	Vec<T, N> result;
	for(size_t i = 0; i < N; i++) { result[i] = Math::Clamp(v[i], min[i], max[i]); }
	return result;
}

template<typename T, size_t N>
constexpr bool Vec<T, N>::InRange(const Vec<T, N> &min, const Vec<T, N> &max) const
{
	for(size_t i = 0; i < N; i++) { if(!Math::InRange(e[i], min[i], max[i])) return false; }
	return true;
}

template<typename T, size_t N>
void Vec<T, N>::ForAll(function_view<void(size_t)> func) const
{
	for(size_t i = 0; i < N; i++) { func(i); }
}