#pragma once

#include <GL/glew.h>

namespace GL
{
	enum BufferType
	{
		ArrBuffer     = GL_ARRAY_BUFFER,
		ElemArrBuffer = GL_ELEMENT_ARRAY_BUFFER,
	};

	enum ClearBit
	{
		ColorBit   = GL_COLOR_BUFFER_BIT,
		DepthBit	  = GL_DEPTH_BUFFER_BIT,
		StencilBit = GL_STENCIL_BUFFER_BIT,
	};

	enum Compilaiton
	{
		InfoLogLength  = GL_INFO_LOG_LENGTH,
		CompileStatus  = GL_COMPILE_STATUS,
		DeleteStatus   = GL_DELETE_STATUS,
		LinkStatus     = GL_LINK_STATUS,
		ValidateStatus = GL_VALIDATE_STATUS,
	};

	enum DrawType
	{
		DynamicDraw = GL_DYNAMIC_DRAW,
		StaticDraw  = GL_STATIC_DRAW,
		StreamDraw  = GL_STREAM_DRAW,
	};

	enum PrimType
	{
		Points        = GL_POINTS,
		Lines         = GL_LINES,
		LineStrip     = GL_LINE_STRIP,
		LineLoop      = GL_LINE_LOOP,
		Triangles     = GL_TRIANGLES,
		TriangleStrip = GL_TRIANGLE_STRIP,
		TriangleFan   = GL_TRIANGLE_FAN,
	};

	enum DataType
	{
		//Data Types
		UByte                = GL_UNSIGNED_BYTE,
		Byte                 = GL_BYTE,
		UShort               = GL_UNSIGNED_SHORT,
		Short                = GL_SHORT,
		UInt                 = GL_UNSIGNED_INT,
		Int                  = GL_INT,
		HalfFloat            = GL_HALF_FLOAT,
		Float                = GL_FLOAT,
		UShort565            = GL_UNSIGNED_SHORT_5_6_5,
		UShort4444           = GL_UNSIGNED_SHORT_4_4_4_4,
		UShort5551           = GL_UNSIGNED_SHORT_5_5_5_1,
		UInt210101010Rev     = GL_UNSIGNED_INT_2_10_10_10_REV,
		UInt101111Rev        = GL_UNSIGNED_INT_10F_11F_11F_REV,
		UInt5999Rev          = GL_UNSIGNED_INT_5_9_9_9_REV,
		UInt248              = GL_UNSIGNED_INT_24_8,
		Float32UInt248Rev    = GL_FLOAT_32_UNSIGNED_INT_24_8_REV,

		//Uniform Types
		//Float              = GL_FLOAT,
		Vec2                 = GL_FLOAT_VEC2,
		Vec3                 = GL_FLOAT_VEC3,
		Vec4                 = GL_FLOAT_VEC4,
		Double               = GL_DOUBLE,
		Dvec2                = GL_DOUBLE_VEC2,
		Dvec3                = GL_DOUBLE_VEC3,
		Dvec4                = GL_DOUBLE_VEC4,
		//Int                = GL_INT,
		Ivec2                = GL_INT_VEC2,
		Ivec3                = GL_INT_VEC3,
		Ivec4                = GL_INT_VEC4,
		//UInt               = GL_UNSIGNED_INT,
		Uvec2                = GL_UNSIGNED_INT_VEC2,
		Uvec3                = GL_UNSIGNED_INT_VEC3,
		Uvec4                = GL_UNSIGNED_INT_VEC4,
		Bool                 = GL_BOOL,
		Bvec2                = GL_BOOL_VEC2,
		Bvec3                = GL_BOOL_VEC3,
		Bvec4                = GL_BOOL_VEC4,
		Mat2                 = GL_FLOAT_MAT2,
		Mat3                 = GL_FLOAT_MAT3,
		Mat4                 = GL_FLOAT_MAT4,
		Mat2x3               = GL_FLOAT_MAT2x3,
		Mat2x4               = GL_FLOAT_MAT2x4,
		Mat3x2               = GL_FLOAT_MAT3x2,
		Mat3x4               = GL_FLOAT_MAT3x4,
		Mat4x2               = GL_FLOAT_MAT4x2,
		Mat4x3               = GL_FLOAT_MAT4x3,
		Dmat2                = GL_DOUBLE_MAT2,
		Dmat3                = GL_DOUBLE_MAT3,
		Dmat4                = GL_DOUBLE_MAT4,
		Dmat2x3              = GL_DOUBLE_MAT2x3,
		Dmat2x4              = GL_DOUBLE_MAT2x4,
		Dmat3x2              = GL_DOUBLE_MAT3x2,
		Dmat3x4              = GL_DOUBLE_MAT3x4,
		Dmat4x2              = GL_DOUBLE_MAT4x2,
		Dmat4x3              = GL_DOUBLE_MAT4x3,
		Sampler1D            = GL_SAMPLER_1D,
		Sampler2D            = GL_SAMPLER_2D,
		Sampler3D            = GL_SAMPLER_3D,
		SamplerCube          = GL_SAMPLER_CUBE,
		Sampler1DShadow      = GL_SAMPLER_1D_SHADOW,
		Sampler2DShadow      = GL_SAMPLER_2D_SHADOW,
		Sampler1DArray       = GL_SAMPLER_1D_ARRAY,
		Sampler2DArray       = GL_SAMPLER_2D_ARRAY,
		Sampler1DArrayShadow = GL_SAMPLER_1D_ARRAY_SHADOW,
		Sampler2DArrayShadow = GL_SAMPLER_2D_ARRAY_SHADOW,
		Sampler2DMS          = GL_SAMPLER_2D_MULTISAMPLE,
		Sampler2DMSArray     = GL_SAMPLER_2D_MULTISAMPLE_ARRAY,
		SamplerCubeShadow    = GL_SAMPLER_CUBE_SHADOW,
		SamplerBuffer        = GL_SAMPLER_BUFFER,
		Sampler2DRect        = GL_SAMPLER_2D_RECT,
		Sampler2DRectShadow  = GL_SAMPLER_2D_RECT_SHADOW,
		Isampler1D           = GL_INT_SAMPLER_1D,
		Isampler2D           = GL_INT_SAMPLER_2D,
		Isampler3D           = GL_INT_SAMPLER_3D,
		IsamplerCube         = GL_INT_SAMPLER_CUBE,
		Isampler1DArray      = GL_INT_SAMPLER_1D_ARRAY,
		Isampler2DArray      = GL_INT_SAMPLER_2D_ARRAY,
		Isampler2DMS         = GL_INT_SAMPLER_2D_MULTISAMPLE,
		Isampler2DMSArray    = GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
		IsamplerBuffer       = GL_INT_SAMPLER_BUFFER,
		Isampler2DRect       = GL_INT_SAMPLER_2D_RECT,
		Usampler1D           = GL_UNSIGNED_INT_SAMPLER_1D,
		Usampler2D           = GL_UNSIGNED_INT_SAMPLER_2D,
		Usampler3D           = GL_UNSIGNED_INT_SAMPLER_3D,
		UsamplerCube         = GL_UNSIGNED_INT_SAMPLER_CUBE,
		Usampler1DArray      = GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,
		Usampler2DArray      = GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,
		Usampler2DMS         = GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,
		Usampler2DMSArray    = GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
		UsamplerBuffer       = GL_UNSIGNED_INT_SAMPLER_BUFFER,
		Usampler2DRect       = GL_UNSIGNED_INT_SAMPLER_2D_RECT,
		Image1D              = GL_IMAGE_1D,
		Image2D              = GL_IMAGE_2D,
		Image3D              = GL_IMAGE_3D,
		Image2DRect          = GL_IMAGE_2D_RECT,
		ImageCube            = GL_IMAGE_CUBE,
		ImageBuffer          = GL_IMAGE_BUFFER,
		Image1DArray         = GL_IMAGE_1D_ARRAY,
		Image2DArray         = GL_IMAGE_2D_ARRAY,
		Image2DMS            = GL_IMAGE_2D_MULTISAMPLE,
		Image2DMSArray       = GL_IMAGE_2D_MULTISAMPLE_ARRAY,
		Iimage1D             = GL_INT_IMAGE_1D,
		Iimage2D             = GL_INT_IMAGE_2D,
		Iimage3D             = GL_INT_IMAGE_3D,
		Iimage2DRect         = GL_INT_IMAGE_2D_RECT,
		IimageCube           = GL_INT_IMAGE_CUBE,
		IimageBuffer         = GL_INT_IMAGE_BUFFER,
		Iimage1DArray        = GL_INT_IMAGE_1D_ARRAY,
		Iimage2DArray        = GL_INT_IMAGE_2D_ARRAY,
		Iimage2DMS           = GL_INT_IMAGE_2D_MULTISAMPLE,
		Iimage2DMSArray      = GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY,
		Uimage1D             = GL_UNSIGNED_INT_IMAGE_1D,
		Uimage2D             = GL_UNSIGNED_INT_IMAGE_2D,
		Uimage3D             = GL_UNSIGNED_INT_IMAGE_3D,
		Uimage2DRect         = GL_UNSIGNED_INT_IMAGE_2D_RECT,
		UimageCube           = GL_UNSIGNED_INT_IMAGE_CUBE,
		UimageBuffer         = GL_UNSIGNED_INT_IMAGE_BUFFER,
		Uimage1DArray        = GL_UNSIGNED_INT_IMAGE_1D_ARRAY,
		Uimage2DArray        = GL_UNSIGNED_INT_IMAGE_2D_ARRAY,
		Uimage2DMS           = GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE,
		Uimage2DMSArray      = GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY,
		AtomicUInt           = GL_UNSIGNED_INT_ATOMIC_COUNTER,
	};
}

#define STORAGE_BUFFER_TYPE GL_SHADER_STORAGE_BUFFER
namespace GL
{
	class StorageBuffer
	{
		uint byteSize;
		GL::DrawType usage;
	
	public:
		uint id;
	
		StorageBuffer(uint binding, size_t dataByteSize, void *data, GL::DrawType usage = GL::StreamDraw)
		{
			byteSize = dataByteSize;
			this->usage = usage;
	
			glGenBuffers(1, &id);
			glBindBuffer(STORAGE_BUFFER_TYPE, id);
			glBufferData(STORAGE_BUFFER_TYPE, dataByteSize, data, usage);
			glBindBufferBase(STORAGE_BUFFER_TYPE, binding, id);
			glBindBuffer(STORAGE_BUFFER_TYPE, 0);
		}
	
		void SetData(void* data)
		{
			glBindBuffer(STORAGE_BUFFER_TYPE, id);
			glBufferSubData(STORAGE_BUFFER_TYPE, 0, byteSize, data);
			glBindBuffer(STORAGE_BUFFER_TYPE, 0);
		}
	
		void SetData(void* data, uint offset, uint dataByteSize)
		{
			glBindBuffer(STORAGE_BUFFER_TYPE, id);
			glBufferSubData(STORAGE_BUFFER_TYPE, offset, dataByteSize, data);
			glBindBuffer(STORAGE_BUFFER_TYPE, 0);
		}
	
		float *Map(uint offset = 0, uint dataByteSize = -1)
		{
			if (dataByteSize == -1) { dataByteSize = byteSize; }

			glBindBuffer(STORAGE_BUFFER_TYPE, id);

			return (float*)glMapBufferRange(STORAGE_BUFFER_TYPE, 0, byteSize, GL_MAP_READ_BIT);
		}
	
		void Unmap()
		{
			glUnmapBuffer(STORAGE_BUFFER_TYPE);
			glBindBuffer(STORAGE_BUFFER_TYPE, 0);
		}
	};
}