#pragma once

#include "Debug.h"
#include "GLTypes.h"
#include "MathGL.h"
#include "Mat.h"
#include "Pool.h"
#include "Vec.h"

#include "GL/glew.h"

#include <any>
#include <assert.h>
#include <unordered_map>
#include <vector>

#undef GetProp

//Shader Types
namespace GL
{
	typedef uint ShadID;
	typedef uint ProgID;

	enum class ShaderType { Compute = GL_COMPUTE_SHADER, Fragment = GL_FRAGMENT_SHADER, Geometry = GL_GEOMETRY_SHADER, Vertex = GL_VERTEX_SHADER };

	typedef std::vector<std::string> ShaderPaths;

	struct Uniform
	{
		int ID;
		DataType Type;
	};

	struct UniformData
	{
		UniformData(float value) : Float_(value) {}
		UniformData(const Vector2 &value) : Vector2_(&value) {}
		UniformData(const Vector3 &value) : Vector3_(&value) {}
		UniformData(const Vector4 &value) : Vector4_(&value) {}
		UniformData(int value) : Int_(value) {}
		UniformData(uint value) : Uint_(value) {}
		UniformData(bool value) : Bool_(value) {}
		UniformData(const Math::Mat4 &value) : Mat_(&value) {}

		~UniformData() {}

		union
		{
			const float Float_;
			const Vector2 *Vector2_;
			const Vector3 *Vector3_;
			const Vector4 *Vector4_;
			const int Int_;
			const uint Uint_;
			const bool Bool_;
			const Math::Mat4 *Mat_;
		};
	};
}

//TODO: Separate Program from uniform values, so that multiple materials with different values can point to one shader.
namespace GL
{
	class Program;
	class ProgramPool;
	typedef std::shared_ptr<Program> ProgRef;

	extern ProgRef activeProgram;
	inline ProgRef ActiveProgram() { return activeProgram; }

	typedef std::unordered_map<std::string, DataType> PropertyMap;
	class Program
	{
	public:
		inline int GetID(const std::string &uniformName) const;
		inline Uniform GetUniform(const std::string &uniformName) const;

		inline void Use() const;
		static void Push(const Program &prog), Pop();

		inline void Dispatch() const;

		///<summary> Sets a uniform by name. </summary>
		inline void Set(const std::string &name, UniformData value) const;
		///<summary> Sets a uniform by ID. Faster than by name, if used together with GetUniform(). </summary>
		inline void Set(const Uniform &unif, UniformData value) const;

		inline static void SetGlobal(const std::string &name, UniformData value);

		const ProgID ID;

	private:
		enum class ProgramType { Graphical, Compute };

		template<typename, typename>
		friend class Pool;
		friend class ProgramPool;

		Program(const ShaderPaths &shaderPaths, ProgramPool *pool)
			: ID(LoadProgram(shaderPaths)), properties(GetProgramUniforms(ID)), pool(pool)
		{
			int matrices = glGetUniformBlockIndex(ID, "Matrices");
			if(matrices != -1) { glUniformBlockBinding(ID, matrices, 0); }

			for(size_t i = 0; i < shaderPaths.size(); i++)
			{ shaders.push_back(GetShaderType(shaderPaths[i])); }

			if(shaders.size() == 1 && shaders[0] == ShaderType::Compute)
			{ type = ProgramType::Compute; }
			else { type = ProgramType::Graphical; }
		}


		PropertyMap GetProgramUniforms(uint program) const;
		DataType GetProp(const std::string &name) const;
		ShaderType GetShaderType(const std::string &path) const;

		std::string LoadTxtFile(const std::string &filePath) const;
		ShadID CreateShader(const std::string &src, GL::ShaderType shaderType, const std::string &filePath) const;
		ProgID CreateProgram(const std::vector<ShadID> &shaderIDs) const;
		ProgID LoadProgram(const ShaderPaths &shaderInfos) const;


		ProgramPool *pool;
		PropertyMap properties;
		ProgramType type;
		std::vector<ShaderType> shaders;
	};

	class ProgramPool : private Pool<ProgramPool, Program>
	{
	public:
		enum DefaultProgram
		{
			eSolid3D,
			eTex3D,

			eScreen,
			eSolidTex,
			eDefaultTex,
			eScreenTex,
			eRampTex,

			MAX
		};

		ProgRef Load(const ShaderPaths &paths)
		{ return ((Pool *)this)->Load(paths); }

		ProgRef LoadByName(const std::string &name, const ShaderPaths &paths)
		{ return ((Pool *)this)->LoadByName(name, paths); }

		ProgRef Get(const std::string &name)
		{ return ((Pool *)this)->Get(name); }


		ProgRef LoadDefaultProgram(DefaultProgram libShader);

	private:
		template<typename, typename>
		friend class Pool;
		friend Program;

		size_t GetHash(const ShaderPaths &paths)
		{
			std::string concPaths = "";
			for(uint i = 0; i < paths.size(); i++)
			{ concPaths += paths[i]; }

			return std::hash<std::string>{}(concPaths);
		}


		std::vector<std::string> tags = { "Graphics", "Shader" };
		static const std::array<ShaderPaths, DefaultProgram::MAX> LibShaders;
	};
}

namespace GL
{
	inline void Program::Use() const { glUseProgram(ID); activeProgram = std::make_shared<Program>(*this); }

	inline void Program::Dispatch() const
	{
		if(type != ProgramType::Compute)
		{
			Debug::Log("A program without a compute shader cannot be dispatched!", Debug::Error, { "Graphics", "Shader", "Compute" });
			return;
		}

		//glDispatchCompute()
	}

	inline DataType Program::GetProp(const std::string &name) const
	{
		auto it = properties.find(name);
		if(it != properties.end())
		{ return it->second; }
		else
		{ 
			Debug::Log("The property " + std::string(name) + " is not part of the shader's uniforms!", Debug::Error, { "Graphics", "Shader" }); 
			return (DataType)-1;
		}
	}

	inline int Program::GetID(const std::string &uniformName) const
	{ return glGetUniformLocation(ID, uniformName.c_str()); }

	inline Uniform Program::GetUniform(const std::string &uniformName) const
	{ 
		auto prop = GetProp(uniformName);
		if(prop == (DataType)-1)
		{ return Uniform{ -1, prop }; }

		return Uniform{ GetID(uniformName), prop };
	}

	inline void Program::Set(const std::string &name, UniformData value) const
	{ 
		Uniform unif = GetUniform(name); 
		if(unif.ID == -1) { return; }

		Set(unif, value);
	}	

	inline void Program::Set(const Uniform &unif, UniformData value) const
	{
		if(activeProgram == nullptr || activeProgram->ID != ID)
		{
			Debug::Log("The program has to be active for it's uniforms to be changed!", Debug::Error, { "Graphics", "Shader" });
			return;
		}

		switch(unif.Type)
		{
		case DataType::Float: { auto v = value.Float_  ; glUniform1f (unif.ID, v							); break; }
		case DataType::Vec2:  { auto v = value.Vector2_; glUniform2f (unif.ID, v->x, v->y				); break; }
		case DataType::Vec3:  { auto v = value.Vector3_; glUniform3f (unif.ID, v->x, v->y, v->z		); break; }
		case DataType::Vec4:  { auto v = value.Vector4_; glUniform4f (unif.ID, v->x, v->y, v->z, v->w); break; }
		case DataType::Int:   { auto v = value.Int_    ; glUniform1i (unif.ID, v							); break; }
		case DataType::UInt:  { auto v = value.Uint_   ; glUniform1ui(unif.ID, v							); break; }
		case DataType::Bool:  { auto v = value.Bool_   ; glUniform1i (unif.ID, v							); break; }

		case DataType::Mat2:  { auto v = value.Mat_; assert(v->Rows == 2 && v->Cols == 2);
			glUniformMatrix2fv(unif.ID, 1, false, &v->v[0]); break; }

		case DataType::Mat3:  { auto v = value.Mat_; assert(v->Rows == 3 && v->Cols == 3);
			glUniformMatrix3fv(unif.ID, 1, false, &v->v[0]); break; }

		case DataType::Mat4:  { auto v = value.Mat_; assert(v->Rows == 4 && v->Cols == 4);
			glUniformMatrix4fv(unif.ID, 1, false, &v->v[0]); break; }
		default: UNDEFINED_CODE_PATH
		}
	}

	inline void Program::SetGlobal(const std::string &name, UniformData value)
	{ activeProgram->Set(name, value); }
}
