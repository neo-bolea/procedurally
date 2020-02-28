#include "GL.h"

#include "Debug.h"
#include "MathGL.h"
#include "Shader.h"

#include <GL/glew.h>

#define NOMINMAX
#include <fstream>
#include <Shlwapi.h>

#pragma comment(lib,"shlwapi.lib")

namespace GL
{
	void GLAPIENTRY
		ErrorCallback( GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam )
	{
		if(type != GL_DEBUG_TYPE_ERROR) { return; }
		Debug::Log(std::string("OpenGL: ") 
			+ "Type = " + std::to_string(type) 
			+ ", Severity = " + std::to_string(severity)
			+ ", " + (char *)message, Debug::Error, { "Graphics" });
	}

	#undef StaticGLGetInteger
	#define StaticGLGetInteger(name, handle) \
		int name()									  \
		{												  \
			static int name = -1;				  \
			if(name == -1)							  \
			{ glGetIntegerv(handle, &name); }  \
			return name;							  \
		}

	StaticGLGetInteger(TextureMaxSize, GL_MAX_TEXTURE_SIZE)
	StaticGLGetInteger(Texture3DMaxSize, GL_MAX_3D_TEXTURE_SIZE)
}

namespace GLHelper
{
	uint CreateUBO(uint bindingPoint, uint size, GL::DrawType drawType)
	{
		uint ubo;
		glGenBuffers(1, &ubo);

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, (int)drawType);

		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);

		return ubo;
	}

	void BindUBOData(uint offset, uint size, void *data)
	{ glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data); }

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
		
		//Generate buffers
		uint VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		
		//Bind buffers and data
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		GLHelper::SetVBOData<std::vector<Vector2>>(arr, 2, 0);
		
		//Draw
		glBindVertexArray(VAO);
		glDrawArrays(mode, 0, (int)arr.size());
		glBindVertexArray(0);
		
		//Delete
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		
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