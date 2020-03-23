#pragma once

#include "Graphics/Texture.h"
#include "Common/Vec.h"

namespace Graphics
{
	void DrawCube(fVec2 pos, fVec2 size, fVec3 color, GL::Tex2DRef &texture = GL::Tex2DRef());
}