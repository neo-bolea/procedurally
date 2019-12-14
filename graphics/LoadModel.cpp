#include "StringUtils.h"
#include "Vec.h"

#include <charconv>
#include <fstream>
#include <string>
#include <vector>

//void Load(const std::string& path, std::vector<fVec3>& vertices, std::vector<fVec3>& normals)
//{
//	std::vector<fVec3> indexedVertices;
//	std::vector<fVec3> indexedNormals;
//	std::vector<int>   vertIndices, normalIndices;
//
//	std::ifstream file(path);
//	std::string   currLine;
//	while (std::getline(file, currLine))
//	{
//		size_t wordStart = 0, wordEnd = 1;
//		if (currLine[0] == 'v')
//		{
//			std::vector<std::string_view> xyz;
//			xyz.reserve(3);
//			Strings::Split1(' ', xyz, currLine, currLine.find_first_of(' ') + 1);
//
//			if (currLine[1] == ' ')
//			{
//				fVec3 v;
//				for (size_t i = 0; i < xyz.size(); i++)
//				{
//					std::from_chars(xyz[i].data(), xyz[i].data() + xyz[i].size(), v[i]);
//				}
//				indexedVertices.push_back(v);
//			}
//			else if (currLine[1] == 'n')
//			{
//				fVec3 v;
//				for (size_t i = 0; i < xyz.size(); i++)
//				{
//					std::from_chars(xyz[i].data(), xyz[i].data() + xyz[i].size(), v[i]);
//				}
//				indexedNormals.push_back(v);
//			}
//		}
//		else if (currLine[0] == 'f' && currLine[1] == ' ')
//		{
//			std::vector<std::string_view> mixedIndices;
//			Strings::Split1(' ', mixedIndices, currLine, currLine.find_first_of(' ') + 1);
//
//			std::vector<std::vector<std::string_view>> singleIndices;
//			singleIndices.resize(mixedIndices.size());
//			for (int i = 0; i < mixedIndices.size(); i++)
//			{
//				Strings::Split1('/', singleIndices[i], mixedIndices[i]);
//				int v, vn;
//				std::from_chars(singleIndices[i][0].data(),
//					singleIndices[i][0].data() + singleIndices[i][0].size(), v);
//				std::from_chars(singleIndices[i][2].data(),
//					singleIndices[i][2].data() + singleIndices[i][2].size(), vn);
//				vertIndices.push_back(v - 1);
//				normalIndices.push_back(vn - 1);
//			}
//		}
//	}
//
//	// Since .obj adds different indices for each attribute, and OpenGL only accepts one pair of
//	// indices, we have to "expand" the indices into what they are pointing to.
//	vertices.reserve(vertIndices.size());
//	for (size_t i = 0; i < vertIndices.size(); i++)
//	{
//		vertices.push_back(indexedVertices[vertIndices[i]]);
//	}
//
//	normals.reserve(normalIndices.size());
//	for (size_t i = 0; i < normalIndices.size(); i++)
//	{
//		normals.push_back(indexedNormals[normalIndices[i]]);
//	}
//}