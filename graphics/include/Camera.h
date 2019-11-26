#pragma once

#include "Mat.h"
#include "Vec.h"

class Camera
{
public:
	Vector3 Pos, Front;
	float Yaw, Pitch, FOV;
	float Near, Far;

	Camera(Vector3 pos, Vector3 front, float yaw = 0.f, float pitch = 0.f, float fov = 60.f, float near = 0.01f, float far = 1000.f)
		: Pos(pos), Front(front), Yaw(yaw), Pitch(pitch), FOV(fov), Near(near), Far(far) {}

	void Update(float speed, float lockView, float horizMove, float vertMove, float horizMouse, float vertMouse, float scroll, float deltaTime);

	//Math::Mat4 GetProjection(), GetView();
};