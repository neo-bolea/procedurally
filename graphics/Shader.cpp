#include "Shader.h"

#include "GLTypes.h"

#include <fstream>
#include <stack>

//Program
namespace GL
{
	ProgRef activeProgram;

	Program::Program(const ShaderPaths &shaderPaths, ProgramPool *pool)
	: ID(LoadProgram(shaderPaths)), properties(GetProgramUniforms(ID)), pool(pool)
	{
		int matrices = glGetUniformBlockIndex(ID, "Matrices");
		if(matrices != -1) { glUniformBlockBinding(ID, matrices, 0); }

		for(size_t i = 0; i < shaderPaths.size(); i++)
		{ shaders.push_back(GetShaderType(shaderPaths[i])); }

		if(shaders.size() == 1 && shaders[0] == ShaderType::Compute)
		{ 
			type = ProgramType::Compute;
			glGetProgramiv(ID, GL_COMPUTE_WORK_GROUP_SIZE, localComputeWorkGroupSize.data());

			int maxWorkGroupSize[3];
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxWorkGroupSize[0]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxWorkGroupSize[1]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxWorkGroupSize[2]);
			for (size_t i = 0; i < 3; i++)
			{
				localComputeWorkGroupSize[i] = 
					std::min(localComputeWorkGroupSize[i], maxWorkGroupSize[i]);
			}
		}
		else { type = ProgramType::Graphical; }
	}

	Program::Program(const Program &other)
		: ID(other.ID), properties(other.properties), pool(pool)
	{
		type = other.type;
		shaders = other.shaders;
		localComputeWorkGroupSize = other.localComputeWorkGroupSize;
	}

	/*
			ProgramPool *pool;
		PropertyMap properties;
		ProgramType type;
		std::vector<ShaderType> shaders;

		std::unordered_set<std::string> erroneousUniforms;

		std::unique_ptr<int[]> localComputeWorkGroupSize;
	*/


	void Program::Use() const
	{
		glUseProgram(ID);
		activeProgram = std::make_shared<Program>(*this);
	}

	void Program::Dispatch(uint x, uint y, uint z) const
	{
		if (type != ProgramType::Compute)
		{
			log("A program without a compute shader cannot be dispatched.",
				Debug::Error, { "Graphics", "Shader", "Compute" });
			return;
		}

		if (x == 0 || y == 0 || z == 0)
		{
			log("A program of size 0 was dispatched - all dimensions must be larger than 0.",
				Debug::Warning, { "Graphics", "Shader", "Compute" });
		}

		if (activeProgram == nullptr || activeProgram->ID != ID)
		{
			log("The program has to be active for it to be dispatched.",
				Debug::Error, { "Graphics", "Shader", "Compute" });
			return;
		}

		x = Math::NextPowerOfTwo(x / localComputeWorkGroupSize[0]);
		y = Math::NextPowerOfTwo(y / localComputeWorkGroupSize[1]);
		z = Math::NextPowerOfTwo(z / localComputeWorkGroupSize[2]);

		x = std::max<int>(1, x); 
		y = std::max<int>(1, y); 
		z = std::max<int>(1, z); 

		glDispatchCompute(x, y, z);
	}

	DataType Program::GetProp(const std::string &name)
	{
		auto it = properties.find(name);
		if (it != properties.end())
		{
			return it->second;
		}
		else
		{
			log("The property " + std::string(name)
				+ " is not part of the shader's uniforms!", Debug::Error, { "Graphics", "Shader" });
			return (DataType)-1;
		}
	}

	int Program::GetID(const std::string& uniformName) const
	{
		return glGetUniformLocation(ID, uniformName.c_str());
	}

	Uniform Program::GetUniform(const std::string& uniformName)
	{
		auto prop = GetProp(uniformName);
		if (prop == (DataType)-1)
		{
			return Uniform{ -1, prop };
		}

		return Uniform{ GetID(uniformName), prop };
	}

	void Program::Set(const std::string& name, UniformData value)
	{
		Uniform unif = GetUniform(name);
		if (unif.ID == -1) { return; }

		Set(unif, value);
	}

	void Program::Set(const Uniform& unif, UniformData value) const
	{
		if (activeProgram == nullptr || activeProgram->ID != ID)
		{
			log("The program has to be active for it's uniforms to be changed.",
				Debug::Error, { "Graphics", "Shader" });
			return;
		}

		switch (unif.Type)
		{
		case DataType::Float: { auto v = value.Float_; glUniform1f (unif.ID, v); break; }
		case DataType::Vec2: { auto v = value.Vector2_; glUniform2f (unif.ID, v->x, v->y); break; }
		case DataType::Vec3: { auto v = value.Vector3_; glUniform3f (unif.ID, v->x, v->y, v->z); break; }
		case DataType::Vec4: { auto v = value.Vector4_; glUniform4f (unif.ID, v->x, v->y, v->z, v->w); break; }
		case DataType::Int: { auto v = value.Int_; glUniform1i (unif.ID, v); break; }
		case DataType::UInt: { auto v = value.Uint_; glUniform1ui(unif.ID, v); break; }
		case DataType::Bool: { auto v = value.Bool_; glUniform1i (unif.ID, v); break; }

		case DataType::Mat2: { auto v = value.Mat_; assert(v->Rows == 2 && v->Cols == 2);
			glUniformMatrix2fv(unif.ID, 1, false, &v->v[0]); break; }

		case DataType::Mat3: { auto v = value.Mat_; assert(v->Rows == 3 && v->Cols == 3);
			glUniformMatrix3fv(unif.ID, 1, false, &v->v[0]); break; }

		case DataType::Mat4: { auto v = value.Mat_; assert(v->Rows == 4 && v->Cols == 4);
			glUniformMatrix4fv(unif.ID, 1, false, &v->v[0]); break; }
		default: UNDEFINED_CODE_PATH
		}
	}

	void Program::SetGlobal(const std::string& name, UniformData value)
	{
		activeProgram->Set(name, value);
	}


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
			log("Could not open shader at " + filePath,
				Debug::Error, { "Graphics", "Shader", "IO" });
			return "";
		}

		return shaderCode;
	}

	ShadID Program::CreateShader(
		const std::string &src,
		GL::ShaderType shaderType, 
		const std::string &filePath) const
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
			{ log(filePath + ": \n" + errorStr,
				Debug::Error, { "Graphics", "Shader", "IO" }); }
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

			log(errorStr, Debug::Error, { "Graphics", "Shader", "IO" } );
			return -1;
		}

		for(uint i = 0; i < shaderIDs.size(); i++)
		{
			glDetachShader(programID, shaderIDs[i]);
			glDeleteShader(shaderIDs[i]);
		}

		return programID;
	}

	const std::unordered_map<ShaderType, std::string> shaderTypeStrings
	{ 
		{ ShaderType::Compute, "compute" }, { ShaderType::Fragment, "fragment" },
		{ ShaderType::Geometry, "geometry" }, { ShaderType::Vertex, "vertex" } 
	};

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
				log("Not able to load " + shaderTypeStrings.at(type) + " shader: " + shaderInfos[i],
					Debug::Error, { "Graphics", "Shader", "IO" });
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
			glGetActiveUniform(program, i, MAX_PROPERTY_NAME_LENGTH, &length, &size, 
				(GLenum *)&prop, buffer);
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