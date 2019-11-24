#pragma once

#include "Types.h"
#include "Array2D.h"

namespace Math
{
	namespace GL
	{
		Vector3 Cross(const Vector3 &v1, const Vector3 &v2);
		float Dot(const Vector3 &v1, const Vector3 &v2);

		Array2D<float> Identity(uint size);
		Array2D<float> Inverse(const Array2D<float> &m);
		Array2D<float> LookAt(Vector3 const &eye, Vector3 const &center, Vector3 const &up);
		Array2D<float> Orthogonal(float left, float right, float bottom, float top, float near, float far);
		Array2D<float> Perspective(float fovy, float aspect, float zNear, float zFar);
		Array2D<float> Rotate(Array2D<float> &m, float angle, const Vector3 &v);
		Array2D<float> Scale(Array2D<float> const &m, Vector3 const &v);
		Array2D<float> Translate(Array2D<float> const &m, Vector3 const &v);
	}
}