#include "Graphics/GL.h"

#include "Common/Debug.h"
#include "Graphics/MathGL.h"
#include "Graphics/Shader.h"

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
}