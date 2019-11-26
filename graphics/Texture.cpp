#include "Texture.h"

#include "GL.h"

#include <stack>

namespace GL
{
	std::unordered_map<Channel, uchar> channelToNr =
	{
		{ Red , 1 },
		{ RG	, 2 },
		{ RGB	, 3 },
		{ RGBA, 4 },

		{ RedInt , 1 },
		{ RGInt	, 2 },
		{ RGBInt	, 3 },
		{ RGBAInt, 4 },
	};

	std::unordered_map<uchar, Channel> nrToChannel =
	{
		{ 1, Red  },
		{ 2, RG	 },
		{ 3, RGB	 },
		{ 4, RGBA },
	};

	std::unordered_map<Channel, Channel> sizedToBasedFormat =
	{
		{ DepthComponent, DepthComponent },
		{ DepthStencil  , DepthStencil   },
		{ Red           , Red            },
		{ RG	          , RG	            },
		{ RGB	          , RGB	         },
		{ RGBA          , RGBA           },
	
		{ R8	          , Red  },
		{ R8_SNORM	    , Red  },
		{ R16				 , Red  },
		{ R16_SNORM		 , Red  },
		{ RG8				 , RG   },
		{ RG8_SNORM		 , RG   },
		{ RG16	       , RG   },
		{ RG16_SNORM	 , RG   },
		{ R3_G3_B2	    , RGB  },
		{ RGB4	       , RGB  },
		{ RGB5	       , RGB  },
		{ RGB8	       , RGB  },
		{ RGB8_SNORM	 , RGB  },
		{ RGB10	       , RGB  },
		{ RGB12	       , RGB  },
		{ RGB16_SNORM	 , RGB  },
		{ RGBA2	       , RGB  },
		{ RGBA4	       , RGB  },
		{ RGB5_A1	    , RGBA },
		{ RGBA8	       , RGBA },
		{ RGBA8_SNORM	 , RGBA },
		{ RGB10_A2	    , RGBA },
		{ RGB10_A2UI	 , RGBA },
		{ RGBA12			 , RGBA },
		{ RGBA16			 , RGBA },
		{ SRGB8	       , RGB  },
		{ SRGB8_ALPHA8	 , RGBA },
		{ R16F	       , Red  },
		{ RG16F	       , RG	  },
		{ RGB16F			 , RGB  },
		{ RGBA16F	    , RGBA },
		{ R32F	       , Red  },
		{ RG32F	       , RG	  },
		{ RGB32F			 , RGB  },
		{ RGBA32F	    , RGBA },
		{ R11F_G11F_B10F, RGB  },
		{ RGB9_E5	    , RGB  },
		{ R8I				 , RedInt  },
		{ R8UI	       , RedInt  },
		{ R16I	       , RedInt  },
		{ R16UI	       , RedInt  },
		{ R32I	       , RedInt  },
		{ R32UI	       , RedInt  },
		{ RG8I	       , RGInt   },
		{ RG8UI	       , RGInt   },
		{ RG16I	       , RGInt   },
		{ RG16UI			 , RGInt   },
		{ RG32I	       , RGInt   },
		{ RG32UI			 , RGInt   },
		{ RGB8I	       , RGBInt  },
		{ RGB8UI			 , RGBInt  },
		{ RGB16I			 , RGBInt  },
		{ RGB16UI	    , RGBInt  },
		{ RGB32I			 , RGBInt  },
		{ RGB32UI	    , RGBInt  },
		{ RGBA8I			 , RGBAInt },
		{ RGBA8UI	    , RGBAInt },
		{ RGBA16I	    , RGBAInt },
		{ RGBA16UI	    , RGBAInt },
		{ RGBA32I	    , RGBAInt },
		{ RGBA32UI	    , RGBAInt },
	};

	static std::unordered_map<uint, uint> texToBinding =
	{
		{ GL_TEXTURE_1D, GL_TEXTURE_BINDING_1D },
		{ GL_TEXTURE_2D, GL_TEXTURE_BINDING_1D },
		{ GL_TEXTURE_3D, GL_TEXTURE_BINDING_1D },
		{ GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BINDING_CUBE_MAP },
	};

	void Tex::Gen(uint type)
	{
		glGenTextures(1, &ID);
		this->type = type;

		auto iter = texToBinding.find(type);
		if(iter == texToBinding.end())
		{
			Debug::Log("Invalid texture type: " + std::to_string(type), Debug::Error, { "Graphics", "Texture" });
			return;
		}
		bindingType = iter->second;
	}

	void Tex::InitFormat(Channel format)
	{
		//Format
		{
			Format = format;
			auto iter = sizedToBasedFormat.find(Format);
			if(iter == sizedToBasedFormat.end())
			{
				Debug::Log("Invalid sized format for texture: " + std::to_string(Format), Debug::Error, { "Graphics", "Texture" });
				return;
			}
			basedFormat = iter->second;
			NrChannels = channelToNr[basedFormat];
		}
	}

	//<Texture Type, ID>
	std::stack<std::tuple<uint, uint>> bindStack;
	void Tex::PushBind()
	{
		int texPrior;
		glGetIntegerv(bindingType, &texPrior);
		bindStack.push(std::tuple(type, texPrior));

		glBindTexture(type, ID);
	}

	void Tex::PopBind()
	{
		auto[type, id] = bindStack.top();
		glBindTexture(type, id);
	}

	//void Tex::Copy(const Tex &src)
	//{
	//	bindingType		 = src.bindingType	 ;
	//	type				 = src.type				 ;
	//	basedFormat		 = src.basedFormat	 ;
	//	ID              = src.ID             ;
	//	NrChannels      = src.NrChannels     ;
	//	Format          = src.Format         ;
	//	Data            = src.Data           ;
	//	Filter          = src.Filter         ;
	//	Wrap            = src.Wrap           ;
	//	GenerateMipmaps = src.GenerateMipmaps;
	//}
	//
	//Tex::Tex(Tex &&other)
	//{
	//	Copy(other);
	//
	//	other.type = 0;
	//	other.ID = 0;
	//}
	//
	//Tex &Tex::operator =(Tex &&other)
	//{
	//	if(this != &other)
	//	{
	//		glDeleteTextures(1, &ID);
	//
	//		Copy(other);
	//
	//		other.type = 0;
	//		other.ID = 0;
	//	}
	//}


	void Tex1D::Setup(int w, Channel format, void *data)
	{
		Size = w;

		Gen(GL_TEXTURE_1D);
		InitFormat(format);

		PushBind();

		//Generate texture
		glTexImage1D(type, 0, format, w, 0, basedFormat, (GLenum)Data, data);

		//Generate mipmaps
		if(GenerateMipmaps)
		{ 
			glTexParameteri(type, GL_TEXTURE_BASE_LEVEL, MipmapBaseLevel);
			glTexParameteri(type, GL_TEXTURE_MAX_LEVEL, MipmapMaxLevel);
			glGenerateMipmap(type);
		}

		//Wrap mode
		glTexParameteri(type, GL_TEXTURE_WRAP_S, Wrap);

		//Filter mode	 
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, Filter);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, Filter);

		PopBind();
	}

	void Tex1D::Setup(const std::string &path)
	{
		Gen(GL_TEXTURE_1D);

		PushBind();

		//Load texture
		int nrChannels, height;
		uchar *texData = Load(path.c_str(), &Size, &height, &nrChannels);

		if(height != 1)
		{
			Debug::Log("Height of the loaded 1D texture cannot be greater than 1: " + path, Debug::Error);
			return;
		}

		if(height > GL::TextureMaxSize())
		{
			Debug::Log("Width of the loaded 1D texture cannot be greater than " + std::to_string(GL::TextureMaxSize()) + ": " + path, Debug::Error);
			return;
		}

		{
			Format = nrToChannel[nrChannels];
			InitFormat(Format);
		}

		glTexImage1D(type, 0, Format, Size, 0, basedFormat, (GLenum)Data, texData);
		Free(texData);

		//Generate mipmaps
		if(GenerateMipmaps)
		{ 
			glTexParameteri(type, GL_TEXTURE_BASE_LEVEL, MipmapBaseLevel);
			glTexParameteri(type, GL_TEXTURE_MAX_LEVEL, MipmapMaxLevel);
			glGenerateMipmap(type);
		}

		//Wrap mode
		glTexParameteri(type, GL_TEXTURE_WRAP_S, Wrap);

		//Filter mode	 
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, Filter);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, Filter);

		PopBind();
	}

	void Tex2D::Setup(int w, int h, Channel format, void *data)
	{
		Size = iVec2(w, h);
		int maxSize = GL::TextureMaxSize();
		if(Size.x > maxSize || Size.y > maxSize)
		{
			Debug::Log("Dimensions of a 2D texture cannot be greater than " + std::to_string(maxSize), Debug::Error);
			return;
		}

		Gen(GL_TEXTURE_2D);
		InitFormat(format);

		PushBind();

		//Generate texture
		glTexImage2D(type, 0, Format, w, h, 0, basedFormat, (GLenum)Data, data);

		//Generate mipmaps
		if(GenerateMipmaps)
		{ 
			glTexParameteri(type, GL_TEXTURE_BASE_LEVEL, MipmapBaseLevel);
			glTexParameteri(type, GL_TEXTURE_MAX_LEVEL, MipmapMaxLevel);
			glGenerateMipmap(type);
		}

		//Wrap mode
		glTexParameteri(type, GL_TEXTURE_WRAP_S, Wrap);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, Wrap);

		//Filter mode
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, Filter);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, Filter);

		PopBind();
	}

	void Tex2D::Setup(const std::string &path, const uint forcedNrChannels, const Channel forcedFormat)
	{
		Gen(GL_TEXTURE_2D);

		PushBind();

		//Load texture
		int nrChannels;
		uchar *texData = Load(path.c_str(), &Size.x, &Size.y, &nrChannels, forcedNrChannels);
			
		int maxSize = GL::TextureMaxSize();
		if(Size.x > maxSize || Size.y > maxSize)
		{
			Debug::Log("Dimensions of a 2D texture cannot be greater than " + std::to_string(maxSize) + ": " + path, Debug::Error);
			return;
		}

		InitFormat(forcedFormat ? forcedFormat : nrToChannel[nrChannels]);

		glTexImage2D(type, 0, Format, Size.x, Size.y, 0, basedFormat, (GLenum)Data, (void *)texData);
		Free(texData);

		//Generate mipmaps
		if(GenerateMipmaps)
		{ 
			glTexParameteri(type, GL_TEXTURE_BASE_LEVEL, MipmapBaseLevel);
			glTexParameteri(type, GL_TEXTURE_MAX_LEVEL, MipmapMaxLevel);
			glGenerateMipmap(type);
		}

		//Wrap mode
		glTexParameteri(type, GL_TEXTURE_WRAP_S, Wrap);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, Wrap);

		//Filter mode
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, Filter);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, Filter);

		PopBind();
	}

	void Tex3D::Setup(int w, int h, int depth, Channel format, void *data)
	{
		Size = iVec3(w, h, depth);
		int maxSize = GL::Texture3DMaxSize();
		if(Size.x > maxSize || Size.y > maxSize || Size.z > maxSize)
		{
			Debug::Log("Dimensions of a 3D texture cannot be greater than " + std::to_string(maxSize), Debug::Error);
			return;
		}

		Gen(GL_TEXTURE_3D);
		InitFormat(format);

		PushBind();

		//Generate texture
		glTexImage3D(type, 0, format, w, h, depth, 0, basedFormat, (GLenum)Data, data);

		//Generate mipmaps
		if(GenerateMipmaps)
		{ 
			glTexParameteri(type, GL_TEXTURE_BASE_LEVEL, MipmapBaseLevel);
			glTexParameteri(type, GL_TEXTURE_MAX_LEVEL, MipmapMaxLevel);
			glGenerateMipmap(type);
		}

		//Wrap mode
		glTexParameteri(type, GL_TEXTURE_WRAP_S, Wrap);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, Wrap);
		glTexParameteri(type, GL_TEXTURE_WRAP_R, Wrap);

		//Filter mode
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, Filter);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, Filter);

		PopBind();
	}

	void Cubemap::Setup(int w, int h, Channel format, void *data)
	{
		Size = iVec2(w, h);
		int maxSize = GL::TextureMaxSize();
		if(Size.x > maxSize || Size.y > maxSize)
		{
			Debug::Log("Dimensions of a cubemap cannot be greater than " + std::to_string(maxSize), Debug::Error);
			return;
		}

		Gen(GL_TEXTURE_CUBE_MAP);
		InitFormat(format);

		PushBind();

		for(uint i = 0; i < 6; i++)
		{ 
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, w, h, 0, basedFormat, Data, data);
		}

		//Wrap mode
		glTexParameteri(type, GL_TEXTURE_WRAP_S, Wrap);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, Wrap);
		glTexParameteri(type, GL_TEXTURE_WRAP_R, Wrap);

		//Filter mode
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, Filter);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, Filter);

		PopBind();
	}

	void Cubemap::Setup(const std::array<std::string, 6> &faces, const uint forcedNrChannels, const Channel forcedFormat)
	{
		Gen(GL_TEXTURE_CUBE_MAP);
		PushBind();

		int width, height, origWidth, origHeight, nrChannels;
		for(uint i = 0; i < 6; i++)
		{
			uchar *texData = Load(faces[i].c_str(), (int *)&width, (int *)&height, (int *)&nrChannels, forcedNrChannels);

			if(i == 0)
			{
				origWidth = width;
				origHeight = height;

				InitFormat(forcedFormat ? forcedFormat : nrToChannel[nrChannels]);
			}
			else if(origWidth != width || origHeight != height)
			{
				Debug::Log("The dimensions of a cubemap's faces must all match!", Debug::Error);
				return;
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, Format, width, height, 0, basedFormat, Data, texData);
			Free(texData);
		}

		Size = iVec2(width, height);
		int maxSize = GL::TextureMaxSize();
		if(Size.x > maxSize || Size.y > maxSize)
		{
			Debug::Log("Dimensions of a cubemap cannot be greater than " + std::to_string(maxSize), Debug::Error);
			return;
		}

		//Wrap mode
		glTexParameteri(type, GL_TEXTURE_WRAP_S, Wrap);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, Wrap);
		glTexParameteri(type, GL_TEXTURE_WRAP_R, Wrap);

		//Filter mode
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, Filter);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, Filter);

		PopBind();
	}
}

namespace GL
{
	std::unordered_map<Framebuffer::Format, Channel> formatToAttachment =
	{
		{ Framebuffer::DepthComponent16 , DepthComponent },
		{ Framebuffer::DepthComponent24 , DepthComponent },
		{ Framebuffer::DepthComponent32f, DepthComponent },
		{ Framebuffer::Depth24Stencil8  , DepthStencil   },
		{ Framebuffer::Depth32fStencil8 , DepthStencil   },
		{ Framebuffer::StencilIndex8    , Stencil	 	    },
	};

	Framebuffer::Framebuffer(int width, int height, Channel colorFormat, Format depthStencilFormat, const Tex2DRef colorBuffer)
	{
		glGenFramebuffers(1, &ID);
		glBindFramebuffer(GL_FRAMEBUFFER, ID);  

		//Generate & attach the color buffer.
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer->ID, 0); 

		if(depthStencilFormat != Format::None)
		{
			//Generate & attach the render buffer.
			glGenRenderbuffers(1, &RBO);
			glBindRenderbuffer(GL_RENDERBUFFER, RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, depthStencilFormat, width, height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, formatToAttachment[depthStencilFormat], GL_RENDERBUFFER, RBO);
		}

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Debug::Log("The created framebuffer is not complete!", Debug::Error, { "Graphics", "Texture" });
			glBindFramebuffer(GL_FRAMEBUFFER, 0);  
			return;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  
	}

	void Framebuffer::Release()
	{ glDeleteFramebuffers(1, &ID); }

	void Blit(Tex2DRef src, uint fbo, ProgRef program)
	{
		program->Use();

		//glViewport(0, 0, Screen::Width, Screen::Height);
		glActiveTexture(GL::Texture0);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindTexture(GL_TEXTURE_2D, src->ID);

		glDisable(GL_DEPTH_TEST);
		//Quad::Draw();
		glEnable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, src->Size.x, src->Size.y);
	}
}