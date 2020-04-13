#pragma once

#include "common/Debug.h"
#include "graphics/GLTypes.h"

#include <unordered_map>

namespace GL
{
	class Mesh : UniqueMessageRaiser
	{
	public:
		Mesh() {}

		Mesh(const Mesh &other) = delete;
		Mesh &operator=(const Mesh &other) = delete;

		Mesh(Mesh &&other);
		Mesh &operator=(Mesh &&other);

		~Mesh();


		template<typename Arr>
		void SetIndices(
			const Arr &data,
			uint dataSize,
			uint index, 
			uint drawType = GL_STATIC_DRAW,
			uint dataType = GL_FLOAT,
			bool normalized = false);

		template<typename Arr>
		void SetVertexAttribute(
			const Arr &data,
			uint dataSize,
			uint index, 
			uint drawType = GL_STATIC_DRAW,
			uint dataType = GL_FLOAT,
			bool normalized = false);

		//TODO: Should count be cached? How to cache it (check for the largest/smallest buffer?)?
		void Use(uint count, GL::PrimType mode = GL::Triangles);
		bool Initialized() { return VAO; }

		std::unordered_map<uint, uint> VBOs;
		uint VAO = 0, EBO = 0;
	};
}

#include "Mesh.inc"