#pragma once

#include "Graphics/Shader.h"

namespace GL
{
	typedef std::shared_ptr<Program> ProgRef;

	typedef std::unordered_map<std::string, DataType> PropertyMap;
	class Program
	{
	private:
		const ProgID ID;
		PropertyMap properties;

		PropertyMap getProgramUniforms(uint program) const;
		DataType GetProp(const std::string &name) const;

		std::string LoadTxtFile(const std::string &filePath) const;
		ShadID createShader(const std::string &src, GL::ShaderType shaderType, const std::string &filePath) const;
		ProgID createProgram(const std::vector<ShadID> &shaderIDs) const;
		ProgID loadProgram(const ShaderPaths &shaderInfos) const;

		Program(const ShaderPaths &shaderPaths)
			: ID(loadProgram(shaderPaths)), properties(getProgramUniforms(ID)) {}

	public:
		void Use() const;
		void Set(const std::string &name, const std::any value) const;
		void Set(const Uniform &id, const std::any value) const;
		static void SetGlobal(DataType type, const std::string &name, const std::any value);

		Uniform GetID(const std::string &uniformName) const;
		Uniform _requireID(const std::string &uniformName, const std::string &funcName) const;
#define RequireID(uniformName) _requireID(uniformName, __func__)
	};
}