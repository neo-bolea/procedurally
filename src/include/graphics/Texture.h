#pragma once

#include "Graphics/GLTypes.h"
#include "Graphics/MathGL.h"
#include "Graphics/Precompiled.h"
#include "Graphics/Shader.h"

#include "GL/glew.h"
#include "stb_image.h"

#include <iostream>

//Enums
namespace GL
{
#pragma region Enums
	enum TexUse { Diffuse, Specular, Normal, Height, MAX };
	const std::string TexTypeStrs[(uint)TexUse::MAX] = { "diffuse", "specular", "normal", "height" };

	enum Channel
	{
		DepthComponent = GL_DEPTH_COMPONENT,	
		Stencil        = GL_STENCIL        ,	
		DepthStencil	= GL_DEPTH_STENCIL  ,
		Red            = GL_RED            ,
		RG	            = GL_RG				  ,
		RGB	         = GL_RGB				  ,
		RGBA           = GL_RGBA			  ,

		RedInt         = GL_RED_INTEGER    ,
		RGInt	         = GL_RG_INTEGER     ,
		RGBInt	      = GL_RGB_INTEGER    ,
		RGBAInt        = GL_RGBA_INTEGER   ,

		R8	            = GL_R8		        ,
		R8_SNORM		   = GL_R8_SNORM		  ,
		R16			   = GL_R16			     ,
		R16_SNORM	   = GL_R16_SNORM	     ,
		RG8			   = GL_RG8			     ,
		RG8_SNORM	   = GL_RG8_SNORM	     ,
		RG16			   = GL_RG16			  ,
		RG16_SNORM	   = GL_RG16_SNORM	  ,
		R3_G3_B2		   = GL_R3_G3_B2		  ,
		RGB4			   = GL_RGB4			  ,
		RGB5			   = GL_RGB5			  ,
		RGB8			   = GL_RGB8			  ,
		RGB8_SNORM	   = GL_RGB8_SNORM	  ,
		RGB10			   = GL_RGB10			  ,
		RGB12			   = GL_RGB12			  ,
		RGB16_SNORM	   = GL_RGB16_SNORM	  ,
		RGBA2			   = GL_RGBA2			  ,
		RGBA4			   = GL_RGBA4			  ,
		RGB5_A1		   = GL_RGB5_A1		  ,
		RGBA8			   = GL_RGBA8			  ,
		RGBA8_SNORM	   = GL_RGBA8_SNORM	  ,
		RGB10_A2		   = GL_RGB10_A2		  ,
		RGB10_A2UI	   = GL_RGB10_A2UI	  ,
		RGBA12		   = GL_RGBA12		     ,
		RGBA16		   = GL_RGBA16		     ,
		SRGB8			   = GL_SRGB8			  ,
		SRGB8_ALPHA8   = GL_SRGB8_ALPHA8   ,
		R16F			   = GL_R16F			  ,
		RG16F			   = GL_RG16F			  ,
		RGB16F		   = GL_RGB16F		     ,
		RGBA16F		   = GL_RGBA16F		  ,
		R32F			   = GL_R32F			  ,
		RG32F			   = GL_RG32F			  ,
		RGB32F		   = GL_RGB32F		     ,
		RGBA32F		   = GL_RGBA32F		  ,
		R11F_G11F_B10F = GL_R11F_G11F_B10F ,	 
		RGB9_E5	      = GL_RGB9_E5	     ,
		R8I		      = GL_R8I		        ,
		R8UI		      = GL_R8UI		     ,
		R16I		      = GL_R16I		     ,
		R16UI		      = GL_R16UI		     ,
		R32I		      = GL_R32I		     ,
		R32UI		      = GL_R32UI		     ,
		RG8I		      = GL_RG8I		     ,
		RG8UI		      = GL_RG8UI		     ,
		RG16I		      = GL_RG16I		     ,
		RG16UI	      = GL_RG16UI	        ,
		RG32I		      = GL_RG32I		     ,
		RG32UI	      = GL_RG32UI	        ,
		RGB8I		      = GL_RGB8I		     ,
		RGB8UI	      = GL_RGB8UI	        ,
		RGB16I	      = GL_RGB16I	        ,
		RGB16UI	      = GL_RGB16UI	     ,
		RGB32I	      = GL_RGB32I	        ,
		RGB32UI	      = GL_RGB32UI	     ,
		RGBA8I	      = GL_RGBA8I	        ,
		RGBA8UI	      = GL_RGBA8UI	     ,
		RGBA16I	      = GL_RGBA16I	     ,
		RGBA16UI	      = GL_RGBA16UI	     ,
		RGBA32I	      = GL_RGBA32I	     ,
		RGBA32UI       = GL_RGBA32UI       ,
	};

	enum TexFilter
	{
		Nearest = GL_NEAREST,
		Linear  = GL_LINEAR,
	};

	enum TexType
	{
		Tex1D_e = GL_TEXTURE_1D,
		Tex2D_e = GL_TEXTURE_2D,
		Tex3D_e = GL_TEXTURE_3D,
		Cubemap_e = GL_TEXTURE_CUBE_MAP,
	};

	//enum TexSwizzle
	//{
	//	SwizzleR = GL_TEXTURE_SWIZZLE_R,
	//	SwizzleG = GL_TEXTURE_SWIZZLE_G,
	//	SwizzleB = GL_TEXTURE_SWIZZLE_B,
	//	SwizzleA = GL_TEXTURE_SWIZZLE_A,
	//};

	enum TexWrap
	{
		Clamp          = GL_CLAMP_TO_EDGE  , 
		MirroredRepeat = GL_MIRRORED_REPEAT,
		Repeat         = GL_REPEAT         ,
	};

	enum Unspecified
	{
		Texture0 = GL_TEXTURE0,
	};
#pragma endregion

	//TODO: Add TexArrays.
	class Tex
	{
	public:
		~Tex() { Release(); }

		static void Bind(TexType type, uint ID) 
		{ 
			glBindTexture(type, ID);
			activeTex = ID;
		}

		void Bind() const 
		{ 
			glBindTexture(type, ID);
			activeTex = ID;
		}
		void Unbind() const
		{ 
			glBindTexture(type, 0); 
			activeTex = 0;
		}

		void Release() const { if (ID != 0) { glDeleteTextures(1, &ID); } }

		uint ID = 0;

		uint NrChannels = 0;
		Channel Format = (Channel)0;
		DataType Data = UByte;
		TexFilter Filter = TexFilter::Linear;
		TexWrap Wrap = Clamp;

		bool GenerateMipmaps = false;
		int MipmapBaseLevel = 0;
		int MipmapMaxLevel = 1000;

	protected:
		void Gen(TexType type), InitFormat(Channel format);


		uchar *Load(const char *path, int *width, int *height, int *nrChannels, int forcedNrChannels = 0)
		{
			stbi_set_flip_vertically_on_load(true);
			unsigned char *data = stbi_load(path, width, height, nrChannels, forcedNrChannels);
			if (!data)
			{
				std::cout << "Failed to load texture" << std::endl;
			}

			return data;
		}

		void Free(uchar *data)
		{
			stbi_image_free(data);
		}

		inline static uint activeTex;

		uint type;
		Channel basedFormat;

	private:
		uint bindingType;
	};

	class Tex1D : public Tex
	{
	public:
		Tex1D() {}
		Tex1D(int w, Channel format, void *data = NULL);
		Tex1D(const std::string &path);

		TexType Type() { return TexType::Tex1D_e; }

		int Size;
	};
	using Tex1DRef = std::shared_ptr<Tex1D>;

	//TODO: Add option to make multisampled Tex2D
	class Tex2D : public Tex
	{
	public:
		Tex2D() {}
		Tex2D(int w, int h, Channel format, void *data = NULL);
		Tex2D(const std::string &path, const uint forcedNrChannels = 0, const Channel forcedFormat = (Channel)0);

		inline float Ratio() { return (float)Size.x / Size.y; }
		TexType Type() { return TexType::Tex2D_e; }

		iVec2 Size;

		bool Multisamples = false;
		uint Samples = 4;
	};
	using Tex2DRef = std::shared_ptr<Tex2D>;

	class Tex3D : public Tex
	{
	public:
		Tex3D() {}
		Tex3D(int w, int h, int depth, Channel format, void *data = NULL);

		TexType Type() { return TexType::Tex3D_e; }

		iVec3 Size;
	};
	using Tex3DRef = std::shared_ptr<Tex3D>;

	class Cubemap : public Tex
	{
	public:
		Cubemap() {}
		Cubemap(int w, int h, Channel format, void *data = NULL);
		Cubemap(const std::array<std::string, 6> &faces, const uint forcedNrChannels = 0, const Channel forcedFormat = (Channel)0);

		TexType Type() { return TexType::Cubemap_e; }

		iVec2 Size;
	};
	using CubemapRef = std::shared_ptr<Cubemap>;
}

//TODO: Add Renderbuffer class

//Frame Buffer
namespace GL
{
	class Framebuffer
	{
	public:
		enum Format
		{
			None              = 0,

			DepthComponent16  = GL_DEPTH_COMPONENT16 ,
			DepthComponent24  = GL_DEPTH_COMPONENT24 ,
			DepthComponent32f = GL_DEPTH_COMPONENT32F,
			Depth24Stencil8   = GL_DEPTH24_STENCIL8  ,
			Depth32fStencil8  = GL_DEPTH32F_STENCIL8 ,
			StencilIndex8     = GL_STENCIL_INDEX8    ,
		};

		Framebuffer() {}
		Framebuffer(int width, int height, Channel colorFormat, Format depthStencilFormat, 
			Tex2D *colorBuffer = nullptr);

		Framebuffer(const Framebuffer &other) = delete;
		Framebuffer &operator=(const Framebuffer &other) = delete;

		Framebuffer(Framebuffer &&other);
		Framebuffer &operator=(Framebuffer &&other);

		~Framebuffer();


		void Bind(), Use();
		void Copy(Framebuffer &other, const GL::Program *program = nullptr);


		static Framebuffer Default;

		uint ID;
		Tex2DRef Tex;
		uint RBO;

		fVec4 BackgroundColor;

	private:
		Framebuffer(uint ID) : ID(ID) {}
};

	using FramebufferRef = Framebuffer *;
}