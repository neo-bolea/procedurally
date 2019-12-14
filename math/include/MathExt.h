#pragma once

#include "Preprocessor.h"
#include "Vec.h"

#include <type_traits>

//// Constants ////
namespace Math
{
	inline constexpr double Deg2Rad = 0.0174532925199433;
	inline constexpr double Rad2Deg = 57.2957795130823209;
}

//// Functions ////
namespace Math
{
	template<typename T>
	constexpr int Ceil (T v) { int i = static_cast<int>(v); if (i < v) { i++; } return i; }
	template<typename T>
	constexpr int Floor(T v) { int i = static_cast<int>(v); if (i > v) { i--; } return i; }
	template<typename T>
	constexpr int Round(T v) { return (v >= 0) ? (v + static_cast<T>(0.5)) : (v - static_cast<T>(0.5)); }

	template<typename T>
	constexpr T Clamp(T n, T min, T max)
	{
		n = (n < min) ? min : n;
		n = (n > max) ? max : n;
		return n;
	}

	template<typename T>
	constexpr T Lerp(T start, T end, T t)
	{
		return start + (end - start) * t;
	}

	/// <summary> Returns a bilinearly interpolated value at (tx, ty) between the points v00, v10, v01, v11. </summary>
	template<typename T>
	constexpr T BiLerp(T v00, T v10, T v01, T v11, T tx, T ty)
	{
		return Lerp(Lerp(v00, v10, tx), Lerp(v01, v11, tx), ty);
	}

	template<typename T>
	constexpr T InvLerp(T start, T end, T v) { return (v - start) / (end - start); }


	template<typename T>
	constexpr T Map(T f, T minFrom, T maxFrom, T minTo, T maxTo)
	{
		return (f - minFrom) / (maxFrom - minFrom) * (maxTo - minTo) + minTo;
	}

	constexpr int NextPowerOfTwo(int x) 
	{
		x--;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return ++x;
	}


	///<summary> Returns -1 for negative, 1 for positive numbers. </summary>
	template <typename T> constexpr int Sign(T n) { return n < T(0) ? T(-1) : T(1); }

	///<summary> Returns -1 for negative, 1 for positive numbers, and 0 for 0. </summary>
	template <typename T> constexpr int Signum(T n) { return (T(0) < n) - (n < T(0)); }


	///<summary> Identical to Lerp. 
	/// Using Linear Interpolation for exponential easing is incorrect, this is just for clarification. </summary>
	template<typename T>
	constexpr T ExpEase(T a, T b, T sharpness) { return Lerp(a, b, sharpness); }

	// See https://gamedev.stackexchange.com/questions/149103/why-use-time-deltatime-in-lerping-functions for more information.
	///<summary> Like Lerp, but is as fast as Time::DeltaTime corrected to exponential "space". </summary>
	template<typename T>
	constexpr T TimedExpEase(T a, T b, T sharpness, long double deltaTime)
	{
		T blend = 1.f - std::powf(1.f - sharpness, (float)deltaTime);
		return Lerp(a, b, blend);
	}

	template<typename T>
	constexpr Vec2<T> DegToVect(float deg)
	{
		float rad = deg * Deg2Rad;
		return Vec2<T>(std::cos(rad), std::sin(rad));
	}

	template<typename T>
	constexpr T VectToDeg(Vec2<T> v) { return std::atan2(v.y, v.x) * Rad2Deg; }

	template<typename T>
	constexpr Vec2<T> Rotate(Vec2<T> v, T d)
	{
		T r = Math::Deg2Rad * d;
		T sinr = sin(r), cosr = cos(r);

		return Vec2<T>(v.x * cosr - v.y * sinr, v.x * sinr + v.y * cosr);
	}
}