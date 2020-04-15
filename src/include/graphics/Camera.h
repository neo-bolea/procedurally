#pragma once

#include "Common/Vec.h"
#include "Math/Mat.h"

class Camera2D
{
public:
	fVec2 Pos;
	float Speed = 1.f, VertSize = 1.f;
	float Near = 0.01f, Far = 1000.f;
	float Ratio;

	Camera2D(float screenRatio);
	void Update(fVec2 movement, float dt);
	void Update();

private:
	Math::Mat4 proj, view, vp;
	uint ubo;
};