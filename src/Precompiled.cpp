#include "Graphics/Precompiled.h"

namespace GL
{
	namespace Shaders
	{
		const char *ScreenTexVert = 
			R"(
				#version 330 core
				layout (location = 0) in vec2 vPos;
				layout (location = 1) in vec2 vUVs;
				
				out vec2 fTexCoords;
				
				void main()
				{
				    gl_Position = vec4(vPos.x, vPos.y, 0.f, 1.f);
				    fTexCoords = vUVs;
				}
			)";

		const char *ScreenTexFrag = TexFrag;
	}

	namespace Shaders
	{
		const char *TexVert = 
			R"(
				#version 330 core
				layout (location = 0) in vec2 vPos;
				layout (location = 1) in vec2 vUVs;
				
				out vec2 fTexCoords;
				
				uniform Matrices
				{
				    mat4 uView;
				    mat4 uProj;
				    mat4 uVP;
				};
				
				uniform mat4 uModel;
				
				void main()
				{
				    gl_Position = uVP * uModel * vec4(vPos.x, vPos.y, 0.f, 1.f);
				    fTexCoords = vUVs;
				}
			)";

		const char *TexFrag = 
			R"(
				#version 330 core

				uniform sampler2D uTex;
				uniform vec4 uColor = ivec4(1.f);
				uniform bool uUseTex = true;
				uniform float uDepth = 0;
				
				out vec4 FragColor;
				
				in vec2 fTexCoords;
				
				void main()
				{
					 gl_FragDepth = uDepth;
				    FragColor = (uUseTex ? vec4(texture(uTex, fTexCoords).xyz, 1.f) : vec4(1.f)) * uColor;
				}
			)";
	}

	namespace Programs
	{
		GL::Program screenTex2DProg;
		const GL::Program &ScreenTex2D()
		{
			static bool once = false;
			if(!once)
			{
				once = true;
				screenTex2DProg.Setup(std::vector<ShaderInfo>{
					{ Shaders::ScreenTexVert, ShaderType::Vertex },
					{ Shaders::ScreenTexFrag, ShaderType::Fragment } 
				});
			}

			return screenTex2DProg;
		}
	}

	namespace Programs
	{
		GL::Program tex2DProg;
		const GL::Program &Tex2D()
		{
			static bool once = false;
			if(!once)
			{
				once = true;
				tex2DProg.Setup(std::vector<ShaderInfo>{
					{ Shaders::TexVert, ShaderType::Vertex },
					{ Shaders::TexFrag, ShaderType::Fragment } 
				});
			}

			return tex2DProg;
		}
	}

	namespace Models
	{
		namespace Square2D
		{
			const std::array<fVec2, 6> Verts =
			{
				fVec2{ -0.5f, -0.5f },
				fVec2{  0.5f, -0.5f },
				fVec2{ -0.5f,  0.5f },

				fVec2{  0.5f, -0.5f },
				fVec2{  0.5f,  0.5f },
				fVec2{ -0.5f,  0.5f },
			};

			const std::array<fVec2, 6> UVs =
			{
				fVec2{ 0.f, 0.f },
				fVec2{ 1.f, 0.f },
				fVec2{ 0.f, 1.f },

				fVec2{ 1.f, 0.f },
				fVec2{ 1.f, 1.f },
				fVec2{ 0.f, 1.f },
			};
		}
	}
}