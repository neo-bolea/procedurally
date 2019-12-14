#include "Camera.h"

#include "MathExt.h"
#include "MathGL.h"
#include "Time.h"

#include <iostream>

void Camera::Update(float speed, bool lockView, const dVec2 &move, const dVec2 &mouseMove, float scroll, double deltaTime)
{
	Vector3 right = (Vector3::Cross(Front, Vector3::Up)).Normalize();
	Vector3 up = (Vector3::Cross(Front, right)).Normalize();
	Pos = Pos + speed * Front * move.y * deltaTime;
	Pos = Pos + speed * right * move.x * deltaTime;
	Pos = Pos + speed * up * scroll * deltaTime;

	if(!lockView)
	{
		Yaw   +=  static_cast<float>(mouseMove.x);
		Pitch +=  static_cast<float>(mouseMove.y);
		FOV   += -scroll;
	}

	Pitch = Math::Clamp(Pitch, -89.5f, 89.5f);

	Front.x = static_cast<float>(cos(Math::Deg2Rad * static_cast<double>(Pitch))
		* cos(Math::Deg2Rad * static_cast<double>(Yaw)));
	Front.y = static_cast<float>(-sin(Math::Deg2Rad * static_cast<double>(Pitch)));
	Front.z = static_cast<float>(cos(Math::Deg2Rad * static_cast<double>(Pitch))
		* sin(Math::Deg2Rad * static_cast<double>(Yaw)));
	Front = Front.Normalize();
}

//Math::fMat4 Camera::GetProjection()
//{ return Math::GL::Perspective(Math::Deg2Rad * FOV, Screen::AspectRatio(), Near, Far); }
//
//Math::fMat4 Camera::GetView()
//{ return Math::GL::LookAt(Pos, Pos + Front, Vector3::Up); }