#pragma once

#include "Debug.h"
#include "GLTypes.h"
#include "MathExt.h"
#include "MathGL.h"
#include "Mat.h"
#include "Pool.h"
#include "Vec.h"

#include "GL/glew.h"

#include <any>
#include <assert.h>
#include <unordered_map>
#include <unordered_set>
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

namespace GL
{
	class Program;
	class ProgramPool;
	typedef std::shared_ptr<Program> ProgRef;

	extern ProgRef activeProgram;
	inline ProgRef ActiveProgram() { return activeProgram; }

	typedef std::unordered_map<std::string, DataType> PropertyMap;
	class Program : UniqueMessageRaiser
	{
	public:
		Program::Program(const Program &other);


		int GetID(const std::string &uniformName) const;
		Uniform GetUniform(const std::string &uniformName);

		void Use() const;
		static void Push(const Program &prog), Pop();

		void Dispatch(uint x, uint y, uint z) const;

		///<summary> Sets a uniform by name. </summary>
		void Set(const std::string &name, UniformData value);
		///<summary> Sets a uniform by ID. Faster than by name, when used together with GetUniform(). </summary>
		void Set(const Uniform &unif, UniformData value) const;

		static void SetGlobal(const std::string &name, UniformData value);

		const ProgID ID;

	private:
		enum class ProgramType { Graphical, Compute };

		template<typename, typename>
		friend class Pool;
		friend class ProgramPool;

		Program(const ShaderPaths &shaderPaths, ProgramPool *pool);


		PropertyMap GetProgramUniforms(uint program) const;
		DataType GetProp(const std::string &name);
		ShaderType GetShaderType(const std::string &path) const;

		std::string LoadTxtFile(const std::string &filePath) const;
		ShadID CreateShader(const std::string &src, GL::ShaderType shaderType, const std::string &filePath) const;
		ProgID CreateProgram(const std::vector<ShadID> &shaderIDs) const;
		ProgID LoadProgram(const ShaderPaths &shaderInfos) const;


		ProgramPool *pool;
		PropertyMap properties;
		ProgramType type;
		std::vector<ShaderType> shaders;

		std::array<int, 3> localComputeWorkGroupSize;
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