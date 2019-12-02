#pragma once

#include "StringUtils.h"

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

using Path = char *;

namespace Graphics
{
	struct Shader
	{
		enum class Use
		{
			Vertex,
			Pixel,
			Compute,
			Tesselation
		};

		const Path Path;
		const Use Type;
	};

	using ShadColl = std::vector<Shader>;

	struct ShadCollHash
	{
		static std::hash<std::string> hasher;
		size_t operator()(const ShadColl &sc)
		{
			size_t hash = 0;
			for(size_t i = 0; i < sc.size(); i++)
			{ hash += hasher(sc[i].Path); }
			return hash;
		}
	};

	struct Program
	{
		using GID = uint;
		using Ref = std::shared_ptr<Program>;

		std::vector<Shader> Shaders;
		GID ID;
	};

	class ProgramManager
	{
	public:
		template<typename ...Arg>
		Program::Ref Create(Arg ...args)
		{
			auto initList = { args };
			std::vector<Path> paths = { initList };
			
		}

	private:
		
		Shader::Use getUse(Path path)
		{
			const std::string ext = Strings::GetExtension(path);
			Shader::Use use;
			if(strcmp(ext.c_str(), ".frag")) { use = Shader::Use::Pixel; }
			else if(strcmp(ext.c_str(), ".frag")) { use = Shader::Use::Pixel; }
			else if(strcmp(ext.c_str(), ".frag")) { use = Shader::Use::Pixel; }
			else if(strcmp(ext.c_str(), ".frag")) { use = Shader::Use::Pixel; }
		}
		std::unordered_map<ShadColl, Program, ShadCollHash> programs;
	};
}