#include "Graphics/Camera.h"

#include "Graphics/GL.h"
#include "Graphics/MathGL.h"

#include "GL/glew.h"


Camera2D::Camera2D(float screenRatio) : Ratio(screenRatio)
{
	ubo = GLHelper::CreateUBO(0, (16 * sizeof(float)) * 3);
	Update();
}

void Camera2D::Update(fVec2 movement, float dt)
{
	Pos += movement * Speed * VertSize * dt;
	Update();
}

void Camera2D::Update()
{
	fVec3 finalPos(Pos.x, Pos.y, -1.f);
	view = Math::GL::LookAt(finalPos, finalPos - fVec3::Forward);
	proj = Math::GL::Orthographic(-VertSize * Ratio, VertSize * Ratio, -VertSize, VertSize, Near, Far);

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	GLHelper::BindUBOData(0, 16 * sizeof(float), &view);
	GLHelper::BindUBOData(16 * sizeof(float), 16 * sizeof(float), &proj);
	GLHelper::BindUBOData(16 * sizeof(float) * 2, 16 * sizeof(float), &(proj * view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}