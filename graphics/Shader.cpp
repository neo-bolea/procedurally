#include "Shader.h"

#include "GLTypes.h"

#include <fstream>
#include <stack>

//Program
namespace GL
{
	ProgRef activeProgram;

	std::string Program::LoadTxtFile(const std::string &filePath) const
	{
		std::string shaderCode;
		std::ifstream shaderStream(filePath, std::ios::in);
		if(shaderStream.is_open())
		{
			std::stringstream sstr;
			sstr << shaderStream.rdbuf();
			shaderCode = sstr.str();
			shaderStream.close();
		}
		else
		{
			Debug::Log("Could not open shader at " + filePath, Debug::Error, { "Graphics", "Shader", "IO" });
			return "";
		}

		return shaderCode;
	}

	ShadID Program::CreateShader(const std::string &src, GL::ShaderType shaderType, const std::string &filePath) const
	{
		ShadID shaderID = glCreateShader((uint)shaderType);

		//Compile Shader
		char const *srcPtr = src.c_str();
		glShaderSource(shaderID, 1, &srcPtr, NULL);
		glCompileShader(shaderID);

		GLint Result = false;
		int logLength;

		//Check Shader
		glGetShaderiv(shaderID, GL::CompileStatus, &Result);
		glGetShaderiv(shaderID, GL::InfoLogLength, &logLength);
		if(logLength > 0) {
			std::vector<char> errorMsg(logLength + 1);
			glGetShaderInfoLog(shaderID, logLength, NULL, &errorMsg[0]);

			std::string errorStr(errorMsg.begin(), errorMsg.end());

			if(!filePath.empty())
			{ Debug::Log(filePath + ": \n" + errorStr, Debug::Error, { "Graphics", "Shader", "IO" }); }
			return -1;
		}

		return shaderID;
	}

	ShadID Program::CreateProgram(const std::vector<ShadID> &shaderIDs) const
	{
		//Link the program
		ProgID programID = glCreateProgram();
		for(uint i = 0; i < shaderIDs.size(); i++) { glAttachShader(programID, shaderIDs[i]); }
		glLinkProgram(programID);

		int result = false;
		int logLength;

		//Check the program
		glGetProgramiv(programID, LinkStatus, &result);
		glGetProgramiv(programID, InfoLogLength, &logLength);
		if(logLength > 0) {
			std::vector<char> errorMsg(logLength + 1);
			glGetProgramInfoLog(programID, logLength, NULL, &errorMsg[0]);

			std::string errorStr(errorMsg.begin(), errorMsg.end());

			Debug::Log(errorStr, Debug::Error, { "Graphics", "Shader", "IO" } );
			return -1;
		}

		for(uint i = 0; i < shaderIDs.size(); i++)
		{
			glDetachShader(programID, shaderIDs[i]);
			glDeleteShader(shaderIDs[i]);
		}

		return programID;
	}

	const std::unordered_map<ShaderType, std::string> shaderTypeStrings{ { ShaderType::Compute, "compute" }, { ShaderType::Fragment, "fragment" },
	{ ShaderType::Geometry, "geometry" }, { ShaderType::Vertex, "vertex" } };

	ShadID Program::LoadProgram(const ShaderPaths &shaderInfos) const
	{
		std::vector<ShadID> shaderIDs;
		shaderIDs.resize(shaderInfos.size());

		//Create the shaders
		for(uint i = 0; i < shaderIDs.size(); i++)
		{
			ShaderType type = GetShaderType(shaderInfos[i]);
			shaderIDs[i] = CreateShader(LoadTxtFile(shaderInfos[i]), type, shaderInfos[i]);
			if(shaderIDs[i] == -1)
			{
				Debug::Log("Not able to load " + shaderTypeStrings.at(type) + " shader: " + shaderInfos[i], Debug::Error, { "Graphics", "Shader", "IO" });
				return -1;
			}
		}

		return CreateProgram(shaderIDs);
	}

#define MAX_PROPERTY_NAME_LENGTH 32
	PropertyMap Program::GetProgramUniforms(uint program) const
	{
		if (ID == -1) { return PropertyMap(); }

		PropertyMap properties;

		GLchar buffer[MAX_PROPERTY_NAME_LENGTH];
		int uniCount, length, size;
		DataType prop;
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniCount);
		for(uint i = 0; i < (uint)uniCount; i++)
		{
			glGetActiveUniform(program, i, MAX_PROPERTY_NAME_LENGTH, &length, &size, (GLenum *)&prop, buffer);
			properties[buffer] = prop;
		}

		return properties;
	}

	ShaderType Program::GetShaderType(const std::string &path) const
	{
		uint lastPos = (uint)path.find_last_of(".");
		std::string extension = path.substr(lastPos, path.size() - lastPos);
		if(extension == ".comp") { return ShaderType::Compute; }
		else if(extension == ".frag") { return ShaderType::Fragment;}
		else if(extension == ".geom") { return ShaderType::Geometry;}
		else if(extension == ".vert") { return ShaderType::Vertex; }
		else { UNDEFINED_CODE_PATH; return (ShaderType)-1; }	//No matching extension was found
	}

	std::stack<uint> bindStack;
	void Program::Push(const Program &prog)
	{
		int shaderPrior;
		glGetIntegerv(GL_CURRENT_PROGRAM, &shaderPrior);
		bindStack.push(shaderPrior);

		prog.Use();
	}

	void Program::Pop()
	{
		int shaderPrior = bindStack.top();
		glUseProgram(shaderPrior);
	}
}

////Default Programs
//namespace GL
//{
//	std::string libShaderPath()
//	{
//		static std::string path = CGE::LibRscsLoc + "shaders\\";
//		return path;
//	}
//
//	const ShaderPaths Solid3DShader{ { "Min3D.vert" }, { "Solid.frag" } };
//	const ShaderPaths Tex3DShader{ { "Min3D.vert" }, { "Tex.frag" } };
//
//	const ShaderPaths ScreenShader{ { "PassThrough.vert" }, { "Solid.frag" } };
//	const ShaderPaths SolidTexShader{ { "Texture.vert" }, { "Solid.frag" } };
//	const ShaderPaths DefaultTexShader{ { "Texture.vert" }, { "Tex.frag" } };
//	const ShaderPaths ScreenTexShader{ { "ScreenPos.vert" }, { "Tex.frag" } };
//	const ShaderPaths TexRampShader{ { "Texture.vert" }, { "TexRamp.frag" } };
//
//	const std::array<ShaderPaths, ProgramPool::MAX> ProgramPool::LibShaders =
//	{
//		Solid3DShader,
//		Tex3DShader,
//
//		ScreenShader,
//		SolidTexShader,
//		DefaultTexShader,
//		ScreenTexShader,
//		TexRampShader,
//	};
//
//	ProgRef ProgramPool::LoadDefaultProgram(ProgramPool::DefaultProgram libShader)
//	{
//		ShaderPaths paths = ProgramPool::LibShaders[(uint)libShader];
//
//		for(size_t i = 0; i < paths.size(); i++)
//		{ paths[i] = CGE::LibRscsLoc + "shaders\\" + paths[i]; }
//
//		return Load(paths);
//	}
//}