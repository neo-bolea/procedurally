#pragma once

#include "Graphics/Texture.h"
#include "Common/Vec.h"

namespace Graphics
{
	void DrawRect(fVec2 pos, fVec2 size, fVec3 color, int depth = 0, GL::Tex2DRef &texture = GL::Tex2DRef());
	void DrawRect(fVec2 pos, fVec2 size, fVec4 color, int depth = 0, GL::Tex2DRef &texture = GL::Tex2DRef());
}