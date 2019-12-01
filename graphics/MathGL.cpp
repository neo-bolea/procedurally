#include "MathGL.h"

#include "Mat.h"

namespace Math
{
	Mat4 GL::Inverse(const Mat4 &m)
	{
		float Coef00 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
		float Coef02 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
		float Coef03 = m[2][1] * m[3][2] - m[2][2] * m[3][1];

		float Coef04 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
		float Coef06 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
		float Coef07 = m[1][1] * m[3][2] - m[1][2] * m[3][1];

		float Coef08 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
		float Coef10 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
		float Coef11 = m[1][1] * m[2][2] - m[1][2] * m[2][1];

		float Coef12 = m[0][2] * m[3][3] - m[0][3] * m[3][2];
		float Coef14 = m[0][1] * m[3][3] - m[0][3] * m[3][1];
		float Coef15 = m[0][1] * m[3][2] - m[0][2] * m[3][1];

		float Coef16 = m[0][2] * m[2][3] - m[0][3] * m[2][2];
		float Coef18 = m[0][1] * m[2][3] - m[0][3] * m[2][1];
		float Coef19 = m[0][1] * m[2][2] - m[0][2] * m[2][1];

		float Coef20 = m[0][2] * m[1][3] - m[0][3] * m[1][2];
		float Coef22 = m[0][1] * m[1][3] - m[0][3] * m[1][1];
		float Coef23 = m[0][1] * m[1][2] - m[0][2] * m[1][1];

		Vector4 Fac0(Coef00, Coef00, Coef02, Coef03);
		Vector4 Fac1(Coef04, Coef04, Coef06, Coef07);
		Vector4 Fac2(Coef08, Coef08, Coef10, Coef11);
		Vector4 Fac3(Coef12, Coef12, Coef14, Coef15);
		Vector4 Fac4(Coef16, Coef16, Coef18, Coef19);
		Vector4 Fac5(Coef20, Coef20, Coef22, Coef23);

		Vector4 Vec0(m[0][1], m[0][0], m[0][0], m[0][0]);
		Vector4 Vec1(m[1][1], m[1][0], m[1][0], m[1][0]);
		Vector4 Vec2(m[2][1], m[2][0], m[2][0], m[2][0]);
		Vector4 Vec3(m[3][1], m[3][0], m[3][0], m[3][0]);

		Vector4 Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
		Vector4 Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
		Vector4 Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
		Vector4 Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

		Vector4 SignA(+1, -1, +1, -1);
		Vector4 SignB(-1, +1, -1, +1);
		Inv0 = Inv0 * SignA;
		Inv1 = Inv1 * SignB;
		Inv2 = Inv2 * SignA;
		Inv3 = Inv3 * SignB;

		Mat4 inverse;
		inverse.v =
		{
			Inv0.x, Inv0.y, Inv0.z, Inv0.w,
			Inv1.x, Inv1.y, Inv1.z, Inv1.w,
			Inv2.x, Inv2.y, Inv2.z, Inv2.w,
			Inv3.x, Inv3.y, Inv3.z, Inv3.w,
		};

		Vector4 Row0(inverse[0][0], inverse[0][1], inverse[0][2], inverse[0][3]);

		Vector4 Dot0(Vector4(m[0][0], m[1][0], m[2][0], m[3][0]) * Row0);
		float Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

		float OneOverDeterminant = 1.f / Dot1;

		return inverse * OneOverDeterminant;
	}

	Mat4 GL::LookAt(const Vector3 &eye, const Vector3 &center, const Vector3 &up)
	{
		fVec3 forward = (center - eye).Normalize();
		fVec3 right = fVec3::Cross(forward, up).Normalize();
		fVec3 relUp = fVec3::Cross(right, forward);

		Mat4 result;
		result[0][0] = right.x;
		result[0][1] = right.y;
		result[0][2] = right.z;
		result[1][0] = relUp.x;
		result[1][1] = relUp.y;
		result[1][2] = relUp.z;
		result[2][0] = -forward.x;
		result[2][1] = -forward.y;
		result[2][2] = -forward.z;
		result[0][3] = -Vector3::Dot(right, eye);
		result[1][3] = -Vector3::Dot(relUp, eye);
		result[2][3] = Vector3::Dot(forward, eye);

		return result;
	}

	Mat4 GL::Orthographic(float left, float right, float bottom, float top, float near, float far)
	{
		Mat4 result;
		result[0][0] = 2.f / (right - left);
		result[1][1] = 2.f / (top - bottom);
		result[2][2] = -2.f / (far - near);
		result[0][3] = -(right + left) / (right - left);
		result[1][3] = -(top + bottom) / (top - bottom);
		result[2][3] = -(far + near) / (far - near);
		return result;
	}

	Mat4 GL::Perspective(float fovy, float aspect, float zNear, float zFar)
	{
		float const tanHalfFovy = tan(fovy / static_cast<float>(2));

		Mat4 result(0.f);
		result[0][0] = static_cast<float>(1) / (aspect * tanHalfFovy);
		result[1][1] = static_cast<float>(1) / (tanHalfFovy);
		result[2][2] = -(zFar + zNear) / (zFar - zNear);
		result[3][2] = -static_cast<float>(1);
		result[2][3] = -(static_cast<float>(2)* zFar* zNear) / (zFar - zNear);
		return result;
	}

	Mat4 GL::Rotate(const Mat4& m, float angle, const Vector3& v)
	{
		float const a = angle, cosa = cos(a), sina = sin(a);

		Vector3 axis = v.Normalize();
		Vector3 temp = (1.f - cosa) * axis;

		Mat4 rotate;
		rotate[0][0] = cosa + temp.x * axis.x;
		rotate[1][0] = temp.x * axis.y + sina * axis.z;
		rotate[2][0] = temp.x * axis.z - sina * axis.y;

		rotate[0][1] = temp.y * axis.x - sina * axis.z;
		rotate[1][1] = cosa + temp.y * axis.y;
		rotate[2][1] = temp.y * axis.z + sina * axis.x;

		rotate[0][2] = temp.z * axis.x + sina * axis.y;
		rotate[1][2] = temp.z * axis.y - sina * axis.x;
		rotate[2][2] = cosa + temp.z * axis.z;

		Vector4 result0(m.vecs[0] * rotate[0][0] + m.vecs[1] * rotate[0][1] + m.vecs[2] * rotate[0][2]);
		Vector4 result1(m.vecs[0] * rotate[1][0] + m.vecs[1] * rotate[1][1] + m.vecs[2] * rotate[1][2]);
		Vector4 result2(m.vecs[0] * rotate[2][0] + m.vecs[1] * rotate[2][1] + m.vecs[2] * rotate[2][2]);

		Mat4 result;
		result.v =
		{
			result0.x, result0.y, result0.z, result0.w,
			result1.x, result1.y, result1.z, result1.w,
			result2.x, result2.y, result2.z, result2.w,
			m[0][3], m[1][3], m[2][3], m[3][3],
		};

		return result;
	}

	Mat4 GL::Scale(const Mat4 &m, const Vector3 &v)
	{
		Mat4 result;
		result.v =
		{
			m[0][0] * v.x, m[1][0] * v.x, m[2][0] * v.x, m[3][0],
			m[0][1] * v.y, m[1][1] * v.y, m[2][1] * v.y, m[3][1],
			m[0][2] * v.z, m[1][2] * v.z, m[2][2] * v.z, m[3][2],
			m[0][3], m[1][3], m[2][3], m[3][3],
		};

		return result;
	}

	Mat4 GL::Translate(const Mat4 &m, const Vector3 &v)
	{
		Mat4 Result(m);
		Vector4 vec = Vector4(m[0][0], m[1][0], m[2][0], m[3][0]) * v.x
			+ Vector4(m[0][1], m[1][1], m[2][1], m[3][1]) * v.y
			+ Vector4(m[0][2], m[1][2], m[2][2], m[3][2]) * v.z
			+ Vector4(m[0][3], m[1][3], m[2][3], m[3][3]);
		Result[0][3] = vec.x;
		Result[1][3] = vec.y;
		Result[2][3] = vec.z;
		Result[3][3] = vec.w;
		return Result;
	}
}