#include "MathGL.h"

const Array2D<float> Math::GL::Identity(1, 0, 0, 0,
													 0, 1, 0, 0,
													 0, 0, 1, 0,
													 0, 0, 0, 1);

Array2D<float> Math::GL::Inverse(const Array2D<float> &m)
{
	float Coef00 = m[2][2] * m[3][3] - m[2][3] * m[3][2];	//Reverse?
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
	
	Vector4 Vec0(m[0][1], m[0][0], m[0][0], m[0][0]);	//Reverse?
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

	Array2D<float> Inverse;
	Inverse.v =
	{
		Inv0.x, Inv0.y, Inv0.z, Inv0.w,	//Reverse?
		Inv1.x, Inv1.y, Inv1.z, Inv1.w,
		Inv2.x, Inv2.y, Inv2.z, Inv2.w,
		Inv3.x, Inv3.y, Inv3.z, Inv3.w,
	};

	Vector4 Row0(Inverse[0][0], Inverse[0][1], Inverse[0][2], Inverse[0][3]);	//Reverse?

	Vector4 Dot0(Vector4(m[0][0], m[1][0], m[2][0], m[3][0]) * Row0);	 //Reverse?
	float Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

	float OneOverDeterminant = 1.f / Dot1;

	return Inverse * OneOverDeterminant;
}

Array2D<float> Math::GL::LookAt(Vector3 const &eye, Vector3 const &center, Vector3 const &up)
{
	Vector3 f((center - eye).Normalize());
	Vector3 const s((Vector3::Cross(f, up)).Normalize());
	Vector3 const u(Vector3::Cross(s, f));
	
	Array2D<float> result = Identity;
	result[0][0] = s.x;
	result[0][1] = s.y;
	result[0][2] = s.z;
	result[1][0] = u.x;
	result[1][1] = u.y;
	result[1][2] = u.z;
	result[2][0] = -f.x;
	result[2][1] = -f.y;
	result[2][2] = -f.z;
	result[0][3] = -Vector3::Dot(s, eye);
	result[1][3] = -Vector3::Dot(u, eye);
	result[2][3] = Vector3::Dot(f, eye);
	return result;
}

Array2D<float> Math::GL::Orthogonal(float left, float right, float bottom, float top, float near, float far)
{
	Array2D<float> Result = Math::GL::Identity;
	Result[0][0] = 2.f / (right - left);
	Result[1][1] = 2.f / (top - bottom);
	Result[2][2] = 1.f / (far - near);
	Result[0][3] = -(right + left) / (right - left);
	Result[1][3] = -(top + bottom) / (top - bottom);
	Result[2][3] = -near / (far - near);
	return Result;
}

Array2D<float> Math::GL::Perspective(float fovy, float aspect, float zNear, float zFar)
{
	assert(std::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.f);

	float const tanHalfFovy = tan(fovy / 2.f);

	Array2D<float> result;
	result[0][0] = 1.f / (aspect * tanHalfFovy);
	result[1][1] = 1.f / (tanHalfFovy);
	result[2][2] = -(zFar + zNear) / (zFar - zNear);
	result[3][2] = -1.f;
	result[2][3] = -(2.f * zFar * zNear) / (zFar - zNear);
	return result;
}

Array2D<float> Math::GL::Rotate(Array2D<float> &m, float angle, const Vector3 &v)
{
	float const a = angle, c = cos(a), s = sin(a);

	Vector3 axis(v.Normalize());
	Vector3 temp((1.f - c) * axis);

	Array2D<float> rotate;
	rotate[0][0] = c + temp.x * axis.x;
	rotate[1][0] = temp.x * axis.y + s * axis.z;
	rotate[2][0] = temp.x * axis.z - s * axis.y;

	rotate[0][1] = temp.y * axis.x - s * axis.z;
	rotate[1][1] = c + temp.y * axis.y;
	rotate[2][1] = temp.y * axis.z + s * axis.x;

	rotate[0][2] = temp.z * axis.x + s * axis.y;
	rotate[1][2] = temp.z * axis.y - s * axis.x;
	rotate[2][2] = c + temp.z * axis.z;

	Vector4 m0(m[0][0], m[1][0], m[2][0], m[3][0]);
	Vector4 m1(m[0][1], m[1][1], m[2][1], m[3][1]);
	Vector4 m2(m[0][2], m[1][2], m[2][2], m[3][2]);
	Vector4 result0(m0 * rotate[0][0] + m1 * rotate[0][1] + m2 * rotate[0][2]);
	Vector4 result1(m0 * rotate[1][0] + m1 * rotate[1][1] + m2 * rotate[1][2]);
	Vector4 result2(m0 * rotate[2][0] + m1 * rotate[2][1] + m2 * rotate[2][2]);

	Array2D<float> result;
	result.v = {
		result0.x, result0.y, result0.z, result0.w,
		result1.x, result1.y, result1.z, result1.w,
		result2.x, result2.y, result2.z, result2.w,
		m[0][3], m[1][3], m[2][3], m[3][3],
	};

	return result;
}

Array2D<float> Math::GL::Scale(Array2D<float> const &m, Vector3 const &v)
{
	Array2D<float> result;
	result.v = {
		m[0][0] * v.x, m[1][0] * v.x, m[2][0] * v.x, m[3][0],
		m[0][1] * v.y, m[1][1] * v.y, m[2][1] * v.y, m[3][1],
		m[0][2] * v.z, m[1][2] * v.z, m[2][2] * v.z, m[3][2],
		m[0][3], m[1][3], m[2][3], m[3][3],
	};
	return result;
}

Array2D<float> Math::GL::Translate(Array2D<float> const &m, Vector3 const &v)
{
	Array2D<float> Result(m);
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