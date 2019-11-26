#pragma once

#include "Mat.h"
#include "Vec.h"

namespace Math
{
	namespace GL
	{
		Mat4 Inverse(const Mat4 &m);
		Mat4 LookAt(Vector3 const &eye, Vector3 const &center, Vector3 const &up);
		Mat4 Orthographic(float left, float right, float bottom, float top, float near, float far);
		Mat4 Perspective(float fovy, float aspect, float zNear, float zFar);
		Mat4 Rotate(Mat4 &m, float angle, const Vector3 &v);
		Mat4 Scale(Mat4 const &m, Vector3 const &v);
		Mat4 Translate(Mat4 const &m, Vector3 const &v);
	}
}