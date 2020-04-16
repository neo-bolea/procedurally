#include "Graphics/Shader.h"

#include "Graphics/GLTypes.h"

#include <boost/container/flat_map.hpp>
#include <fstream>
#include <stack>

//Program
namespace GL
{
	ProgRef activeProgram;
	const ProgRef ActiveProgram() { return activeProgram; }

	Program::Program(const Program &other)
		: ID(other.ID), properties(other.properties)
	{
		type = other.type;
		shaders = other.shaders;
		localComputeWorkGroupSize = other.localComputeWorkGroupSize;
	}

	void Program::setup(std::unordered_set<Rscs::FileRef> &files)
	{	
		std::vector<ShaderInfo> shaderInfos;
		for(auto &file : files)
		{
			shaderInfos.push_back(ShaderInfo(file->Path_, file->Contents, getShaderType(file->Path_)));
		}

		Setup(shaderInfos);
	}

	void Program::Setup(std::vector<ShaderInfo> &shaderInfos)
	{	
		if(previouslyInitialized)
		{
			glDeleteProgram(ID);
			properties.clear();
			shaders.clear();
		}

		for(auto &shader : shaderInfos)
		{ shaders.push_back(shader.Type); }

		ID = setupProgram(shaderInfos);
		PropertyMap &propertyTypes = getProgramUniforms(ID);
		properties.reserve(propertyTypes.size());
		for(auto &propIter : propertyTypes)
		{
			UniformID uniformID = GetID(propIter.first);
			{
				UniformInfo unifInfo = UniformInfo(propIter.first, uniformID, propIter.second);
				DataInfo dataInfo = DataInfo{ propIter.second };
				properties.emplace(unifInfo, dataInfo);
			}
		}

		int matrices = glGetUniformBlockIndex(ID, "Matrices");
		if(matrices != -1) { glUniformBlockBinding(ID, matrices, 0); }

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

	DataType Program::getProperty(const std::string &name) const
	{
		auto it = properties.find(UniformInfo{ name, GetID(name), DataType() });
		if (it != properties.end())
		{
			return it->second.Type;
		}
		else
		{
			log("The property " + std::string(name)
				+ " is not part of the shader's uniforms.", Debug::Error, { "Graphics", "Shader" });
			return (DataType)-1;
		}
	}

	int Program::GetID(const std::string &uniformName) const
	{
		return glGetUniformLocation(ID, uniformName.c_str());
	}

	UniformInfo Program::GetUniform(const std::string &uniformName) const
	{
		DataType prop = getProperty(uniformName);
		if (prop == (DataType)-1)
		{
			return UniformInfo{ "", -1, prop };
		}

		return UniformInfo{ uniformName, GetID(uniformName), prop };
	}

	enum DataClass { Number, Vector, Matrix };
	boost::container::flat_map<DataType, DataClass> DataClasses =
	{
		{ DataType::Float, Number },
		{ DataType::Vec2, Vector },
		{ DataType::Vec3, Vector },
		{ DataType::Vec4, Vector },
		{ DataType::Int, Number },
		{ DataType::UInt, Number },
		{ DataType::Bool, Number },
		{ DataType::Mat2, Matrix },
		{ DataType::Mat3, Matrix },
		{ DataType::Mat4, Matrix },
	};

	void Program::Set(const std::string &name, UniformValue &&value) const
	{
		UniformInfo unif = GetUniform(name);
		if (unif.ID == -1) { return; }

		if(DataClasses[unif.Type] != DataClasses[value.Type])
		{
			log("The given uniform value is not of the same type as the uniform '" + name + "'.",
				Debug::Error, { "Graphics", "Shader" });
			return;
		}

		Set(unif, std::forward<UniformValue>(value));
	}

	void Program::Set(const UniformInfo &unif, UniformValue &&value) const
	{
		if(unif.ID == -1)
		{
			log("The given uniform info (" + unif.Name + ") is not valid.", 
				Debug::Error, { "Graphics", "Shader" });
			return;
		}

		if(DataClasses[unif.Type] != DataClasses[value.Type])
		{
			log("The given uniform (" + unif.Name 
				+ ") value is not of the same type as the uniform with the ID "
				+ std::to_string(unif.ID) + ".", Debug::Error, { "Graphics", "Shader" });
			return;
		}

		if (activeProgram == nullptr || activeProgram->ID != ID)
		{
			log("The program has to be active for it's uniforms to be changed.",
				Debug::Error, { "Graphics", "Shader" });
			return;
		}

#ifdef _DEBUG
		properties[unif].Value = value;
#endif

		switch (unif.Type)
		{
		case DataType::Float: { auto v = value.Float_; glUniform1f(unif.ID, v); break; }
		case DataType::Vec2: { auto v = value.Vector2_; glUniform2f(unif.ID, v->x, v->y); break; }
		case DataType::Vec3: { auto v = value.Vector3_; glUniform3f(unif.ID, v->x, v->y, v->z); break; }
		case DataType::Vec4: { 
			auto v = value.Vector4_; 
			glUniform4f (unif.ID, v->x, v->y, v->z, v->w);
			break; }
		case DataType::Int: { auto v = value.Int_; glUniform1i(unif.ID, v); break; }
		case DataType::UInt: { auto v = value.Uint_; glUniform1ui(unif.ID, v); break; }
		case DataType::Bool: { auto v = value.Bool_; glUniform1i(unif.ID, v); break; }
		case DataType::Mat2: { auto v = value.Mat2; 
			glUniformMatrix2fv(unif.ID, 1, false, reinterpret_cast<const GLfloat *>(v)); break; }
		case DataType::Mat3: { auto v = value.Mat3; 
			glUniformMatrix3fv(unif.ID, 1, false, reinterpret_cast<const GLfloat *>(v)); break; }
		case DataType::Mat4: { auto v = value.Mat4; 
			glUniformMatrix4fv(unif.ID, 1, false, reinterpret_cast<const GLfloat *>(v)); break; }
		default: UNDEFINED_CODE_PATH
		}
	}

	void Program::SetGlobal(const std::string &name, UniformValue &&value)
	{
		activeProgram->Set(name, std::forward<UniformValue>(value));
	}


	std::string Program::loadTxtFile(const std::string &filePath) const
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

	ShadID Program::createShader(
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

	ShadID Program::createProgram(const std::vector<ShadID> &shaderIDs) const
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

	ShadID Program::setupProgram(std::vector<ShaderInfo> &shaderInfos) const
	{
		std::vector<ShadID> shaderIDs;
		shaderIDs.resize(shaderInfos.size());

		int i = 0;
		//Create the shaders
		for(auto &shader : shaderInfos)
		{
			shaderIDs[i] = createShader(shader.Code, shader.Type, shader.Path);
			if(shaderIDs[i] == -1)
			{
				log("Not able to load " + shaderTypeStrings.at(shader.Type) + " shader: " 
					+ shader.Path, Debug::Error, { "Graphics", "Shader", "IO" });
				return -1;
			}

			i++;
		}

		return createProgram(shaderIDs);
	}


#define MAX_PROPERTY_NAME_LENGTH 32
	PropertyMap Program::getProgramUniforms(uint program) const
	{
		if (ID == -1) { return PropertyMap(); }

		PropertyMap propertyMap;

		GLchar buffer[MAX_PROPERTY_NAME_LENGTH];
		int uniCount, length, size;
		DataType prop;
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniCount);
		for(uint i = 0; i < (uint)uniCount; i++)
		{
			glGetActiveUniform(program, i, MAX_PROPERTY_NAME_LENGTH, &length, &size, 
				(GLenum *)&prop, buffer);
			propertyMap[buffer] = prop;
		}

		return propertyMap;
	}

	ShaderType Program::getShaderType(const std::string &path) const
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
