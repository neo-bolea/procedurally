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

		static GL::UniformInfo uModel, uColor, uUseTex, uDepth;
		static GL::Mesh squareMesh;
		if(!squareMesh.Initialized())
		{
			squareMesh.SetVertexAttribute(GL::Models::Square2D::Verts, 2, 0);
			squareMesh.SetVertexAttribute(GL::Models::Square2D::UVs, 2, 1);

			uModel = program.GetUniform("uModel");
			uColor = program.GetUniform("uColor");
			uUseTex = program.GetUniform("uUseTex");
			uDepth = program.GetUniform("uDepth");
		}
		
		GL::Program::Push(program);
		
		Math::Mat4 model;
		model = Math::GL::Translate(model, (fVec3)pos);
		model = Math::GL::Scale(model, (fVec3)size);
		program.Set(uModel, model);
		program.Set(uColor, color);
		program.Set(uUseTex, (bool)texture);
#ifdef _DEBUG
		if(depth <= -maxDepthRange || depth >= maxDepthRange)
		{
			Debug::Log("The integer depth value for drawing must be between -" 
				+ std::to_string(maxDepthRange) + " and " + std::to_string(maxDepthRange) + ".",
				Debug::Warning, { "Graphics" });
			depth = Math::Clamp(depth, -maxDepthRange+1, maxDepthRange-1);
		}
#endif
		program.Set(uDepth, (depth + maxDepthRange) / (maxDepthRange * 2.f));

		squareMesh.Use(GL::Models::Square2D::Verts.size());
		GL::Program::Pop();
	}


	void DebugDrawDC(const std::vector<Vector2> &arr, float size, GLenum mode)
	{
		//Update size/width
		switch(mode)
		{
		case GL::Points: glPointSize(size); break;
		case GL::Lines: glLineWidth(size); break;
		case GL::LineStrip: glLineWidth(size); break;
		case GL::LineLoop: glLineWidth(size); break;
		default:	UNDEFINED_CODE_PATH("The given OpenGL mode is not supported.");
		}

		glDisable(GL_DEPTH_TEST);

		GL::Mesh mesh;
		mesh.SetVertexAttribute(arr, 2, 0);
		mesh.Use(arr.size());

		glEnable(GL_DEPTH_TEST);
	}

	void DebugDrawScreen(const std::vector<Vector2> &screenArr, float size, GLenum mode)
	{
		std::vector<Vector2> dcArr(screenArr.size());
		for(uint i = 0; i < screenArr.size(); i++)
		{ 
			//dcArr[i] = Screen::ScreenToDC(screenArr[i]);
		}
		DebugDrawDC(dcArr, size, mode);
	}
}