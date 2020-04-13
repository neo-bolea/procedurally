#pragma once

#include "Graphics/GL.h"

namespace GL
{
	namespace Shaders
	{
		extern const char *ScreenTexVert;
		extern const char *ScreenTexFrag;

		extern const char *TexVert;
		extern const char *TexFrag;
	}

	namespace Programs
	{
		const GL::Program &Tex2D();
		const GL::Program &ScreenTex2D();
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