#pragma once

#include "Texture.h"
#include "Vec.h"

namespace Graphics
{
	void DrawCube(fVec2 pos, fVec2 size, fVec3 color, GL::Tex2DRef &texture = GL::Tex2DRef());
}