#include "MathTests.h"

#include "Math/Mat.h"
#include "Common/Vec.h"

#include "catch.hpp"

using namespace Math;
using namespace Catch::literals;

TEST_CASE("Vector", "[math][vector]") 
{
	SECTION("Vectors are PODs")
	{
		CHECK(sizeof(Vec<int8, 3>) == sizeof(int8) * 3);
	
		CHECK(sizeof(Vec<real32, 2>) == sizeof(real32) * 2);
		CHECK(sizeof(Vec<real32, 3>) == sizeof(real32) * 3);
		CHECK(sizeof(Vec<real32, 4>) == sizeof(real32) * 4);
		CHECK(sizeof(Vec<real32, 5>) == sizeof(real32) * 5);
		CHECK(sizeof(Vec<real32, 10000>) == sizeof(real32) * 10000);
	}
	
	SECTION("Only vectors of arithmetic types are SIMD-ready")
	{
		CHECK(Vec<int8, 4>::SIMDReady);
		CHECK(Vec<int32, 4>::SIMDReady);
		CHECK(Vec<real32, 4>::SIMDReady);
		CHECK(Vec<real64, 4>::SIMDReady);
		CHECK(Vec<int64, 4>::SIMDReady);
		CHECK(!Vec<std::string, 4>::SIMDReady);
	}
	
	SECTION("Only vectors of size 4 are SIMD-ready")
	{
		CHECK(!Vec<real32, 3>::SIMDReady);
		CHECK(Vec<real32, 4>::SIMDReady);
		CHECK(!Vec<real32, 5>::SIMDReady);
	}
	
	SECTION("Vectors can be accessed with []")
	{
		Vec<real32, 3> v1(1.f, 2.f, 3.f);
		CHECK(v1[0] == 1_a);
		CHECK(v1[1] == 2_a);
		CHECK(v1[2] == 3_a);
	}
	
	SECTION("Vectors can be accessed with xyzw")
	{
		Vec<real32, 4> v1(1.f, 2.f, 3.f, 4.f);
		CHECK(v1.x == 1_a);
		CHECK(v1.y == 2_a);
		CHECK(v1.z == 3_a);
		CHECK(v1.w == 4_a);
	}
	
	
	Vec<real32, 3> v1(-1.f, 0.f, 1034.235f);
	Vec<real32, 3> v2(4.3f, 1.f, 0.23f);
	
	SECTION("Vector + Vector")
	{
		Vec<real32, 3> v3 = v1 + v2;
		CHECK(v3[0] == 3.3_a);
		CHECK(v3[1] == 1_a);
		CHECK(v3[2] == 1034.465_a);
	}
	
	SECTION("Vector - Vector")
	{
		Vec<real32, 3> v3 = v1 - v2;
		CHECK(v3[0] == -5.3_a);
		CHECK(v3[1] == -1_a);
		CHECK(v3[2] == 1034.005_a);
	}
	
	SECTION("Vector * Vector")
	{
		Vec<real32, 3> v3 = v1 * v2;
		CHECK(v3[0] == -4.3_a);
		CHECK(v3[1] == 0.f);
		CHECK(v3[2] == 237.87405_a);
	}
	
	SECTION("Vector * real32")
	{
		Vec<real32, 3> v3 = v1 * 2.3f;
		CHECK(v3[0] == -2.3_a);
		CHECK(v3[1] == 0_a);
		CHECK(v3[2] == 2378.7405_a);
	}
	
	SECTION("Commutative: vector and number")
	{
		Vec<real32, 3> v3 = v1 * 3.1f;
		Vec<real32, 3> v4 = 3.1f * v1;
		CHECK(v3[0] == v4[0]);
		CHECK(v3[1] == v4[1]);
		CHECK(v3[2] == v4[2]);
	}
}

TEST_CASE("Matrix", "[math][matrix]")
{
	Mat<real32, 2, 3> m1(2, 1, 4,
							  0, 1, 1);
	
	SECTION("Matrices are PODs")
	{
		CHECK(sizeof(Mat<int8, 3, 3>) == sizeof(int8) * 3 * 3);
		
		CHECK(sizeof(Mat<real32, 1, 3>) == sizeof(real32) * 1 * 3);
		CHECK(sizeof(Mat<real32, 3, 1>) == sizeof(real32) * 3 * 1);
		CHECK(sizeof(Mat<real32, 100, 1>) == sizeof(real32) * 100 * 1);
		CHECK(sizeof(Mat<real32, 3, 54>) == sizeof(real32) * 3 * 54);
		CHECK(sizeof(Mat<real32, 1000, 1000>) == sizeof(real32) * 1000 * 1000);
	}
	
	SECTION("Matrices can be checked for (un)equality")
	{
		Mat<real32, 2, 3> m2(2, 1, 4,
								  0, 1, 1);
		
		Mat<real32, 2, 3> m3(2, 1, 4,
								  0, 1, 4);
		
		CHECK(m1 == m2);
		CHECK(m1 != m3);
	}
	
	SECTION("Matrices can be accessed with [][]")
	{
		CHECK(m1[0][0] == 2_a);
		CHECK(m1[1][0] == 1_a);
		CHECK(m1[2][0] == 4_a);
		CHECK(m1[0][1] == 0_a);
		CHECK(m1[1][1] == 1_a);
		CHECK(m1[2][1] == 1_a);
	}
	
	
	SECTION("Matrix * Matrix")
	{
		Mat<real32, 2, 3> m1(2, 1, 4,
								  0, 1, 1);
		Mat<real32, 3, 4> m2(6, 3, -1, 0,
								  1, 1, 0, 4,
								  -2, 5, 0, 2);
	
		Mat<real32, 2, 4> m3 = m1 * m2;
		CHECK(m3.Rows == 2);
		CHECK(m3.Cols == 4);
	
		Mat<real32, 2, 4> m4(5, 27, -2, 12,
								  -1, 6, 0, 6);
	
		CHECK(m3 == m4);
	}
}