#pragma once

#include "Graphics/Texture.h"

namespace GL
{
	struct MatTex
	{
		Tex2D Ref;
		TexUse Type_;
	};

	struct Mesh
	{
		void Setup(), Draw();

		std::vector<Vector3> Pos;
		std::vector<Vector2> UVs;
		std::vector<Vector3> Norms;
		std::vector<Vector3> Tans;
		std::vector<Vector3> Bitans;

		std::vector<MatTex> Texs;
		std::vector<uint> Indices;

	private:
		uint VAO, VBO[5], EBO;
	};

	struct Model
	{
	public:
		std::vector<Mesh> meshes;
	private:
		void setup() { for(auto &mesh : meshes) { mesh.Setup(); } }
	public:
		Model() {}
		Model(std::vector<Mesh> meshes) : meshes(meshes) { setup(); }

		void Draw() { for(auto &mesh : meshes) { mesh.Draw(); } }
	};
}