#pragma once

#include "Mat.h"
#include "Vec.h"

namespace Math
{
	namespace GL
	{
		Mat4 Inverse(const Mat4 &m);
		Mat4 LookAt(const Vector3 &eye, const Vector3 &center, const Vector3 &up = Vector3::Up);
		Mat4 Orthographic(float left, float right, float bottom, float top, float near, float far);
		Mat4 Perspective(float fovy, float aspect, float zNear, float zFar);
		Mat4 Rotate(const Mat4 &m, float angle, const Vector3 &v);
		Mat4 Scale(const Mat4 &m, const Vector3 &v);
		Mat4 Translate(const Mat4 &m, const Vector3 &v);
	}
}