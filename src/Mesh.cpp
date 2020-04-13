#include "graphics/Mesh.h"

#include "graphics/Shader.h"

#include "GL/glew.h"

#include <utility>

namespace GL
{
	Mesh::Mesh(Mesh &&other) 
	{
		this->operator=(std::move(other));
	}

	Mesh &Mesh::operator=(Mesh &&other) 
	{
		std::swap(VAO, other.VAO);
		VBOs.swap(other.VBOs);

		return *this;
	}

	Mesh::~Mesh()
	{
		if(VAO) { glDeleteVertexArrays(1, &VAO); }
		if(EBO) { glDeleteBuffers(1, &EBO); }
		for(auto &vbo : VBOs)
		{
			glDeleteBuffers(1, &vbo.second);
		}
	}

	template<typename Arr>
	void Mesh::SetIndices(
		const Arr &data,
		uint dataSize,
		uint index, 
		uint drawType,
		uint dataType,
		bool normalized)
	{
		using T = typename Arr::value_type;

		if(!EBO)
		{
			glGenBuffers(1, &EBO);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), drawType);
	}

	void Mesh::Use(uint count, GL::PrimType mode)
	{
		if(!GL::ActiveProgram()) 
		{
			log("A mesh cannot be used without an active program.",
				Debug::Error, { "Graphics", "Shader" });
			return;
		}

		if(EBO)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glDrawElements(mode, count, GL::UInt, 0);
		}
		else
		{
			glBindVertexArray(VAO);
			glDrawArrays(mode, 0, count);
		}
	}
}