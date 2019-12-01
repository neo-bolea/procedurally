/*#include "GL.h"

#include "Debug.h"
#include "Mesh.h"
#include "Watch.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace GL
{
	void ProcessNode(const aiScene *scene, std::unique_ptr<std::vector<Mesh>> &meshes);
	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<MatTex> LoadMaterialTexs(aiMaterial *mat, aiTextureType type, TexUse texType);

#define FACE_INDICES_COUNT 3

	std::string directory, path;
	std::unordered_map<std::string, MatTex> texturesLoaded;

	//Model LoadModel(const std::string &_path)
	//{
	//	std::unique_ptr<std::vector<Mesh>> meshes = std::unique_ptr<std::vector<Mesh>>(new std::vector<Mesh>());
	//
	//	Assimp::Importer importer;
	//	const aiScene *scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs);
	//	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	//	{
	//		Debug::Log("Unable to load model: " + _path + ", Error:" + importer.GetErrorString(), Debug::Error, 
	//			{ "Assimp", "GL", "IO", "Loading" });
	//		return Model(*meshes.get());
	//	}
	//
	//	path = _path;
	//	directory = _path.substr(0, _path.find_last_of('/'));
	//
	//	ProcessNode(scene, meshes);
	//
	//	return Model(*meshes.get());
	//}

	Model LoadModel(const std::string& _path)
	{
		std::unique_ptr<std::vector<Mesh>> meshes = std::unique_ptr<std::vector<Mesh>>(new std::vector<Mesh>());
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			return Model(*meshes.get());
		}
		path = _path;
		directory = _path.substr(0, _path.find_last_of('/'));

		ProcessNode(scene, meshes);

		return Model(*meshes.get());
	}

	void ProcessNode(const aiScene *scene, std::unique_ptr<std::vector<Mesh>> &meshes)
	{
		std::vector<aiNode*> nodes;
		nodes.push_back(scene->mRootNode);

		aiNode *node;
		while(!nodes.empty())
		{
			node = nodes.back();
			nodes.pop_back();

			for(uint i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes->push_back(ProcessMesh(mesh, scene));
			}

			for(uint i = 0; i < node->mNumChildren; i++)
			{ nodes.push_back(node->mChildren[i]); } }
	}

	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene)
	{
		Mesh result;
		if(mesh->HasPositions())
		{
			//Positions
			result.Pos.reserve(mesh->mNumVertices);
			for(uint i = 0; i < mesh->mNumVertices; i++)
			{ result.Pos.push_back(Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z)); } }

		if(mesh->HasNormals())
		{	//Normals
			result.Norms.reserve(mesh->mNumVertices);
			for(uint i = 0; i < mesh->mNumVertices; i++)
			{ result.Norms.push_back(Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)); } }

		if(mesh->HasTextureCoords(0))
		{	//UVs
			result.UVs.reserve(mesh->mNumVertices);
			for(uint i = 0; i < mesh->mNumVertices; i++)
			{ result.UVs.push_back(Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)); } }

		if(mesh->HasTangentsAndBitangents())
		{	//Tangents
			result.Tans.reserve(mesh->mNumVertices);
			for(uint i = 0; i < mesh->mNumVertices; i++)
			{ result.Tans.push_back(Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z)); } 
			//Bitangents
			result.Bitans.reserve(mesh->mNumVertices);
			for(uint i = 0; i < mesh->mNumVertices; i++)
			{ result.Bitans.push_back(Vector3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z)); } }

		if(mesh->HasFaces())
		{	//Indices
			result.Indices.reserve(mesh->mNumFaces * FACE_INDICES_COUNT);
			for(uint i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for(uint j = 0; j < FACE_INDICES_COUNT; j++)
				{
					result.Indices.push_back(face.mIndices[j]);
				}
			}
		}

		//Materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<MatTex> diffuseMaps = LoadMaterialTexs(material, aiTextureType_DIFFUSE, TexUse::Diffuse);
		result.Texs.insert(result.Texs.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<MatTex> specularMaps = LoadMaterialTexs(material, aiTextureType_SPECULAR, TexUse::Specular);
		result.Texs.insert(result.Texs.end(), specularMaps.begin(), specularMaps.end());
		std::vector<MatTex> normalMaps = LoadMaterialTexs(material, aiTextureType_HEIGHT, TexUse::Normal);
		result.Texs.insert(result.Texs.end(), normalMaps.begin(), normalMaps.end());
		std::vector<MatTex> heightMaps = LoadMaterialTexs(material, aiTextureType_AMBIENT, TexUse::Height);
		result.Texs.insert(result.Texs.end(), heightMaps.begin(), heightMaps.end());
		/*aiMaterial material = scene->mMaterials[index];
		aiString texture_file;
		material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_file);
		if(auto texture = scene->GetEmbeddedTexture(texture_file.C_Str())) {
			//returned pointer is not null, read texture from memory
		}
		else {
			//regular file, check if it exists and read it
		}* /

		return result;
	}

	std::vector<MatTex> LoadMaterialTexs(aiMaterial *mat, aiTextureType type, TexUse texType)
	{
		std::vector<MatTex> textures;
		for(uint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString texName;
			mat->GetTexture(type, i, &texName);

			//If the texture has already been loaded, we don't have to load it again.
			auto loadedTexture = texturesLoaded.find(texName.C_Str());
			if(loadedTexture != texturesLoaded.end())
			{
				textures.push_back(loadedTexture->second);
			}
			else
			{	//The texture hasn't been loaded yet, so load it.
				MatTex matTex;
				matTex.Ref.Filter = Linear;
				matTex.Ref.Wrap = Repeat;
				matTex.Ref.GenerateMipmaps = true;
				matTex.Ref.Setup(directory + "/" + texName.C_Str());

				matTex.Type_ = texType;
				textures.push_back(matTex);
				texturesLoaded.insert({ texName.C_Str(), matTex });
			}
		}
		return textures;
	}
};*/