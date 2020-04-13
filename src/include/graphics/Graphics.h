#pragma once

#include "Common/Vec.h"
#include "Graphics/Texture.h"

namespace Graphics
{
	void DrawRect(fVec2 pos, fVec2 size, fVec3 color, int depth = 0, GL::Tex2DRef &texture = GL::Tex2DRef());
	void DrawRect(fVec2 pos, fVec2 size, fVec4 color, int depth = 0, GL::Tex2DRef &texture = GL::Tex2DRef());

	void DebugDrawDC(const std::vector<Vector2> &arr, float size = 5.f, GLenum mode = GL_POINTS);
	void DebugDrawScreen(const std::vector<Vector2> &screenArr, float size = 5.f, GLenum mode = GL_POINTS);
}