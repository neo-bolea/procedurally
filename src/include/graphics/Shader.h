#pragma once

#include "Common/Debug.h"
#include "Graphics/GLTypes.h"
#include "Math/MathExt.h"
#include "Graphics/MathGL.h"
#include "Math/Mat.h"
#include "Common/Resources/ResourceManager.h"
#include "Common/Vec.h"

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

	using UniformID = int;
	struct UniformInfo
	{
		std::string Name;
		DataType Type;
		UniformID ID;

		UniformInfo() {}
		UniformInfo(const std::string &name, UniformID id) : Name(name), ID(id) {}
		UniformInfo(const std::string &name, UniformID id, const DataType &type) 
			: Name(name), ID(id), Type(type) {}
		UniformInfo(const UniformInfo &other) : Name(other.Name), ID(other.ID), Type(other.Type) {}

		struct Hasher
		{ uint operator()(const UniformInfo &info) const { return info.ID; } };
	};

	static bool operator ==(const UniformInfo &lhs, const UniformInfo &rhs) 
	{ return lhs.ID == rhs.ID; }

	struct UniformValue
	{
		UniformValue() : Type((DataType)-1) {}
		UniformValue(const UniformValue &other) : Mat4(other.Mat4), Type(other.Type) {}

		UniformValue(float value) : Float_(value), Type(DataType::Float) {}
		UniformValue(const Vector2 &value) : Vector2_(&value), Type(DataType::Vec2) {}
		UniformValue(const Vector3 &value) : Vector3_(&value), Type(DataType::Vec3) {}
		UniformValue(const Vector4 &value) : Vector4_(&value), Type(DataType::Vec4) {}
		UniformValue(int value) : Int_(value), Type(DataType::Int) {}
		UniformValue(uint value) : Uint_(value), Type(DataType::UInt) {}
		UniformValue(bool value) : Bool_(value), Type(DataType::Bool) {}
		UniformValue(const Math::Mat2 &value) : Mat2(&value), Type(DataType::Mat2) {}
		UniformValue(const Math::Mat3 &value) : Mat3(&value), Type(DataType::Mat2) {}
		UniformValue(const Math::Mat4 &value) : Mat4(&value), Type(DataType::Mat2) {}

		~UniformValue() {}

		void operator =(const UniformValue &other)
		{
			Mat4 = other.Mat4;
			Type = other.Type;
		}

		union
		{
			const float Float_;
			const Vector2 *Vector2_;
			const Vector3 *Vector3_;
			const Vector4 *Vector4_;
			const int Int_;
			const uint Uint_;
			const bool Bool_;
			const Math::Mat2 *Mat2;
			const Math::Mat3 *Mat3;
			const Math::Mat4 *Mat4;
		};

		DataType Type;
	};


	struct ShaderInfo
	{
		ShaderInfo(const std::string &path, const std::string &code, ShaderType type) 
			: Path(path), Code(code), Type(type) {}
		ShaderInfo(const std::string &code, ShaderType type) 
			: ShaderInfo("Unknown Path", code, type) {}


		std::string Path;
		std::string Code;
		ShaderType Type;
	};
}

//TODO: Add intermediary object for saving Set() commands (e.g. Set("uTex", 1)) into a class, which can act as both a cache and for ease of use.
namespace GL
{
	class Program;
	typedef std::shared_ptr<Program> ProgRef;
	typedef std::shared_ptr<const Program> ProgConstRef;

	const ProgRef ActiveProgram();

	typedef std::unordered_map<std::string, DataType> PropertyMap;
#ifdef _DEBUG
	struct DataInfo { DataType Type; UniformValue Value; };
#else
	struct DataInfo { DataType Type; };
#endif

	class Program : UniqueMessageRaiser, public Rscs::ResourceBase
	{
	public:
		Program() {}
		Program(const Program &other);

		void Setup(std::vector<ShaderInfo> &shaderInfos);

		int GetID(const std::string &uniformName) const;
		UniformInfo GetUniform(const std::string &uniformName) const;

		void Use() const;
		static void Push(const Program &prog), Pop();

		void Dispatch(uint x, uint y, uint z) const;

		///<summary> Sets a uniform by name. </summary>
		void Set(const std::string &name, UniformValue &&value) const;
		///<summary> Sets a uniform by ID. Faster than by name, when used together with GetUniform(). </summary>
		void Set(const UniformInfo &unif, UniformValue &&value) const;


		static void SetGlobal(const std::string &name, UniformValue &&value);

		ProgID ID;

	private:
		enum class ProgramType { Graphical, Compute };


		virtual void setup(std::unordered_set<Rscs::FileRef> &files);

		PropertyMap getProgramUniforms(uint program) const;
		DataType getProperty(const std::string &name) const;
		ShaderType getShaderType(const std::string &path) const;

		std::string loadTxtFile(const std::string &filePath) const;
		ShadID createShader(const std::string &src, 
			GL::ShaderType shaderType, 
			const std::string &filePath) const;
		ProgID createProgram(const std::vector<ShadID> &shaderIDs) const;
		ShadID setupProgram(std::vector<ShaderInfo> &shaderInfos) const;

		mutable std::unordered_map<UniformInfo, DataInfo, UniformInfo::Hasher> properties;
		ProgramType type;
		std::vector<ShaderType> shaders;

		std::array<int, 3> localComputeWorkGroupSize;
	};
}