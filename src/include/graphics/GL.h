#pragma once

#include "Graphics/GLTypes.h"

#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"

#define GL_STATIC
#include "GL/glew.h"

#include <string>

#define GLDEFINED
namespace GL
{
	void GLAPIENTRY
		ErrorCallback(
			GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam);

	namespace
	{
		#define StaticGLGetInteger(name, handle) \
			int name();
	}

	StaticGLGetInteger(TextureMaxSize, GL_MAX_TEXTURE_SIZE)
	StaticGLGetInteger(Texture3DMaxSize, GL_MAX_3D_TEXTURE_SIZE)
}

namespace GLHelper
{
	uint CreateUBO(uint bindingPoint, uint size, GL::DrawType drawType = GL::StaticDraw);
	void BindUBOData(uint offset, uint size, void *data);
}