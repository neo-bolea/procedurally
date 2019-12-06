#define GLEW_STATIC
#define SDL_MAIN_HANDLED
#include "pch.h"
#include "framework.h"
#include "Core.h"

#include "MathExt.h"
#include "Camera.h"
#include "Shader.h"
#include "GL.h"
#include "Systems/Inputs.h"
#include "MathGL.h"
#include "Mesh.h"
#include "StringUtils.h"
#include "Time.h"
#include "Watch.h"

#include "GL/glew.h"
#include "SDL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <charconv>
#include <iostream>
#include <fstream>

#define __WINDOWS_WASAPI__
#define __WINDOWS_DS__
#define __WINDOWS_ASIO__

#define WIDTH (1920)
#define HEIGHT (1080)

void Load(const std::string &path,
	std::vector<fVec3> &vertices, 
	std::vector<fVec3> &normals)
{
	std::vector<fVec3> indexedVertices;
	std::vector<fVec3> indexedNormals;
	std::vector<int> vertIndices, normalIndices;

	std::ifstream file(path);
	std::string currLine;
	while (std::getline(file, currLine))
	{
		size_t wordStart = 0, wordEnd = 1;
		if (currLine[0] == 'v')
		{
			std::vector<std::string_view> xyz;
			xyz.reserve(3);
			Strings::Split1(' ', xyz, currLine, currLine.find_first_of(' ')+1);

			if (currLine[1] == ' ')
			{ 
				fVec3 v;
				for (size_t i = 0; i < xyz.size(); i++)
				{ 
					std::from_chars(xyz[i].data(), xyz[i].data() + xyz[i].size(), v[i]);
				}
				indexedVertices.push_back(v);
			}
			else if (currLine[1] == 'n')
			{ 
				fVec3 v;
				for (size_t i = 0; i < xyz.size(); i++)
				{
					std::from_chars(xyz[i].data(), xyz[i].data() + xyz[i].size(), v[i]);
				}
				indexedNormals.push_back(v);
			}
		}
		else if (currLine[0] == 'f' && currLine[1] == ' ')
		{
			std::vector<std::string_view> mixedIndices;
			Strings::Split1(' ', mixedIndices, currLine, currLine.find_first_of(' ') + 1);

			std::vector<std::vector<std::string_view>> singleIndices;
			singleIndices.resize(mixedIndices.size());
			for (int i = 0; i < mixedIndices.size(); i++)
			{
				Strings::Split1('/', singleIndices[i], mixedIndices[i]);
				int v, vn;
				std::from_chars(singleIndices[i][0].data(), singleIndices[i][0].data() + singleIndices[i][0].size(), v);
				std::from_chars(singleIndices[i][2].data(), singleIndices[i][2].data() + singleIndices[i][2].size(), vn);
				vertIndices.push_back(v - 1);
				normalIndices.push_back(vn - 1);
			}
		}
	}

	// Since .obj adds different indices for each attribute, and OpenGL only accepts one pair of
	// indices, we have to "expand" the indices into what they are pointing to.
	vertices.reserve(vertIndices.size());
	for (size_t i = 0; i < vertIndices.size(); i++)
	{
		vertices.push_back(indexedVertices[vertIndices[i]]);
	}

	normals.reserve(normalIndices.size());
	for (size_t i = 0; i < normalIndices.size(); i++)
	{
		normals.push_back(indexedNormals[normalIndices[i]]);
	}
}

int main(int argc, char *argv[])
{
#pragma region SDL
	//SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	//OpenGL 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	//Anti-Aliasing
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	//Window
	auto window = SDL_CreateWindow("TestTitle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
											 WIDTH, HEIGHT, 
											 SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if(window == NULL)
	{
		std::cout << "Failed to open an SDL window: " << std::endl 
			<< SDL_GetError() << std::endl;
		SDL_Quit();
		return EXIT_FAILURE;
	}

	auto context = SDL_GL_CreateContext(window);
#pragma endregion

#pragma region GLEW
	glewExperimental = GL_TRUE;
	
	if(glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}
	
	glViewport(0, 0, WIDTH, HEIGHT);
	
	//Culling
	glCullFace(GL_NONE);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	
	//Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//Depth Test
	glEnable(GL_DEPTH_TEST);
	
	//Multi Sampling 
	glEnable(GL_MULTISAMPLE);  
	
	//Errors
	glEnable(GL_DEBUG_OUTPUT);

	//VSync
	SDL_GL_SetSwapInterval(0);
#pragma endregion

	GL::Tex2D tex;
	tex.Filter = GL::TexFilter::Linear;
	GL::ProgRef prog = GL::Programs.Load({ "basic.vert", "basic.frag" });
	GL::ProgRef outline = GL::Programs.Load({ "outline.vert", "outline.frag" });

	Watch watch(Watch::ms);
	watch.Start();

	std::vector<fVec3> vertices;
	std::vector<fVec3> normals;
	Load("nanosuit1/scene.obj", vertices, normals);

	watch.Stop();
	std::cout << "Finished: " << watch.sTime() << std::endl;

	uint VAO, VBO[2];
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);

	glBindVertexArray(VAO);

	GLHelper::SetVBOData(VBO[0], vertices, 3, 0);
	GLHelper::SetVBOData(VBO[1], normals, 3, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Inputs inputs;
	Time time;

	bool quit = false;
	SDL_Event event;

	Camera cam(fVec3(0, 0, 0), fVec3::Forward);

	Time::SetLimitFPS(true);

	while(!quit)
	{
		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		dVec2 move;
		move.x = inputs.IsKeyDown(SDL_SCANCODE_D)
			- inputs.IsKeyDown(SDL_SCANCODE_A);
		move.y = inputs.IsKeyDown(SDL_SCANCODE_W)
			- inputs.IsKeyDown(SDL_SCANCODE_S);
		dVec2 mouseMove = inputs.GetMouseMove() * 0.25f;
		cam.Update(1.f, false, move, mouseMove, inputs.GetMouseWheel().x, Time::DeltaTime());
		
		tex.Bind();
		Math::Mat4 model = Math::GL::Scale(Math::Mat4(), fVec3(0.1f));
		Math::Mat4 mvp = model;
		mvp = Math::GL::LookAt(cam.Pos, cam.Pos + cam.Front) * mvp;
		mvp = Math::GL::Perspective(Math::Deg2Rad * 60.f, ((float)WIDTH / HEIGHT), 0.001f, 1000.f) * mvp;

		prog->Use();
		prog->Set("uMVP", mvp);

		glCullFace(GL_BACK);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);

		outline->Use();
		outline->Set("uModel", model);
		outline->Set("uMVP", mvp);
		outline->Set("uEye", (fVec4)cam.Pos);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);

		Locator::Call("Update"); 

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
		
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{ Locator::Call("Inputs/SetKey", event); } break;
			case SDL_MOUSEMOTION:
			{ Locator::Call("Inputs/SetMousePos", event); } break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{ Locator::Call("Inputs/SetMouseButton", event); } break;
			case SDL_MOUSEWHEEL:
			{ Locator::Call("Inputs/SetMouseWheel", event); } break;
			}
		}

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}