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
	template<typename Arr>
	void SetVBOData(
		const Arr &data,
		uint dataSize,
		uint index,
		uint drawType = GL_STATIC_DRAW,
		uint dataType = GL_FLOAT,
		bool normalized = false)
	{
		using T = typename Arr::value_type;
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), drawType);

		glVertexAttribPointer(index, dataSize, dataType, normalized, sizeof(T), (void *)0);
		glEnableVertexAttribArray(index);
	}

	template<typename Arr>
	void SetVBOData(
		uint vbo,
		const Arr &data,
		uint dataSize,
		uint index,
		uint drawType = GL_STATIC_DRAW,
		uint dataType = GL_FLOAT,
		bool normalized = false)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		SetVBOData(data, dataSize, index, drawType, dataType, normalized);
	}

	uint CreateUBO(uint bindingPoint, uint size, GL::DrawType drawType = GL::StaticDraw);
	void BindUBOData(uint offset, uint size, void *data);

	void DebugDrawDC(const std::vector<Vector2> &arr, float size = 5.f, GLenum mode = GL_POINTS);
	void DebugDrawScreen(const std::vector<Vector2> &screenArr, float size = 5.f, GLenum mode = GL_POINTS);
}