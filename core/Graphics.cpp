#include "Graphics/Graphics.h"

#include "Graphics/GL.h"
#include "Graphics/Precompiled.h"

namespace Graphics
{
	void DrawRect(fVec2 pos, fVec2 size, fVec3 color, int depth, GL::Tex2DRef &texture)
	{
		DrawRect(pos, size, fVec4(color.x, color.y, color.z, 1.f), depth, texture);
	}

	const int maxDepthRange = 4194302;
	void DrawRect(fVec2 pos, fVec2 size, fVec4 color, int depth, GL::Tex2DRef &texture)
	{
		const GL::Program &program = GL::Programs::Tex2D();

		static bool once = false;
		static uint vao, vbo;

		if(!once)
		{
			once = true;
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
		
			glBindVertexArray(vao);
			GLHelper::SetVBOData(vbo, GL::Models::Square2D::Verts, 2, 0);
			GLHelper::SetVBOData(vbo, GL::Models::Square2D::UVs, 2, 1);
			glBindVertexArray(0);
		}
		
		GL::Program::Push(program);
		
		Math::Mat4 model;
		//model = Math::GL::Scale(model, (fVec3)fVec2(1.f / size.x, 1.f / size.y));
		model = Math::GL::Translate(model, (fVec3)(pos - fVec2(0.5f) * size));
		model = Math::GL::Scale(model, (fVec3)size);
		program.Set("uModel", model);
		program.Set("uColor", color);
		program.Set("uUseTex", (bool)texture);
#ifdef _DEBUG
		if(depth <= -maxDepthRange || depth >= maxDepthRange)
		{
			Debug::Log("The integer depth value for drawing must be between -" 
				+ std::to_string(maxDepthRange) + " and " + std::to_string(maxDepthRange) + ".",
				Debug::Warning, { "Graphics" });
			depth = Math::Clamp(depth, -maxDepthRange+1, maxDepthRange-1);
		}
#endif
		program.Set("uDepth", (depth + maxDepthRange) / (maxDepthRange * 2.f));
		
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, GL::Models::Square2D::Verts.size());

		GL::Program::Pop();
	}
}