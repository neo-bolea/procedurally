#pragma once

#include "GL.h"

namespace GL
{
	namespace Shaders
	{
		extern const char *TexVert;
		extern const char *TexFrag;
	}

	namespace Programs
	{
		const GL::Program &Tex2D();
	}

	namespace Models
	{
		namespace Square2D
		{
			extern const std::array<fVec2, 6> Verts;
			extern const std::array<fVec2, 6> UVs;
		}
	}
}