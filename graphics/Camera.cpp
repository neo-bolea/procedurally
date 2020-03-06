#include "Camera.h"

#include "GL.h"
#include "MathExt.h"
#include "MathGL.h"
#include "GameTime.h"

#include "GL/glew.h"

#include <iostream>


Camera2D::Camera2D(float screenRatio) : Ratio(screenRatio)
{
	ubo = GLHelper::CreateUBO(0, (16 * sizeof(float)) * 3);
}

void Camera2D::Update(fVec2 movement, float dt)
{
	Pos += (fVec3)movement * Speed * VertSize * dt;
	Pos.z = 1.f;

	view = Math::GL::LookAt(Pos, Pos - fVec3::Forward);
	proj = Math::GL::Orthographic(-VertSize * Ratio, VertSize * Ratio, -VertSize, VertSize, Near, Far);

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	GLHelper::BindUBOData(0, 16 * sizeof(float), &view.v[0]);
	GLHelper::BindUBOData(16 * sizeof(float), 16 * sizeof(float), &proj.v[0]);
	GLHelper::BindUBOData(16 * sizeof(float) * 2, 16 * sizeof(float), &(proj * view).v[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}