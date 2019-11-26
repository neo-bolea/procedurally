#include "Camera.h"

#include "MathExt.h"
#include "MathGL.h"
#include "Time.h"

void Camera::Update(float speed, float lockView, float horizMove, float vertMove, float horizMouse, float vertMouse, float scroll, float deltaTime)
{
	Vector3 right = (Vector3::Cross(Front, Vector3::Up)).Normalize();
	Vector3 up = (Vector3::Cross(Front, right)).Normalize();
	Pos = Pos + speed * Front * vertMove * deltaTime;
	Pos = Pos + speed * right * horizMove * deltaTime;
	//Pos += speed * up    * scroll * (float)Time::DeltaTime();

	if(!lockView)
	{
		Yaw   +=  horizMouse;
		Pitch +=  vertMouse;
		FOV   += -scroll;
	}

	Pitch = Math::Clamp(Pitch, -89.5f, 89.5f);

	Front.x =  cos(Math::Deg2Rad * Pitch) * cos(Math::Deg2Rad * Yaw);
	Front.y = -sin(Math::Deg2Rad * Pitch);
	Front.z =  cos(Math::Deg2Rad * Pitch) * sin(Math::Deg2Rad * Yaw);
	Front = Front.Normalize();
}

//Math::fMat4 Camera::GetProjection()
//{ return Math::GL::Perspective(Math::Deg2Rad * FOV, Screen::AspectRatio(), Near, Far); }
//
//Math::fMat4 Camera::GetView()
//{ return Math::GL::LookAt(Pos, Pos + Front, Vector3::Up); }