#include "Graphics/Precompiled.h"

namespace GL
{
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
				uniform vec4 uColor;
				
				out vec4 FragColor;
				
				in vec2 fTexCoords;
				
				void main()
				{
				    FragColor = texture(uTex, fTexCoords) * vec4(1.f);
				    FragColor.a = 1.f;
				    FragColor.rg = fTexCoords + 1.f;
				    //FragColor.r = float(fTexCoords.x < 1022.f);
				    //FragColor.g = float(fTexCoords.y < 1022.f);
				}
			)";
	}

	namespace Programs
	{
		GL::Program program;
		const GL::Program &Tex2D()
		{
			static bool once = false;
			if(!once)
			{
				once = true;
				program.Setup(std::vector<ShaderInfo>{
					{ Shaders::TexVert, ShaderType::Vertex },
					{ Shaders::TexFrag, ShaderType::Fragment } 
				});
			}

			return program;
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