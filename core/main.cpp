#define GLEW_STATIC
#define SDL_MAIN_HANDLED
#include "pch.h"
#include "framework.h"
#include "Core.h"

#include "MathExt.h"
#include "Camera.h"
#include "IOUtil.h"
#include "Shader.h"
#include "GL.h"
#include "Systems/Inputs.h"
#include "Hash.h"
#include "Mat.h"
#include "MathGL.h"
#include "StringUtils.h"
#include "Time.h"
#include "Vec.h"
#include "Watch.h"

#include "GL/glew.h"
#include "SDL.h"
#include "external/glm/glm.hpp"
#include "external/glm/gtc/matrix_transform.hpp"
#include "external/glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <chrono>
#include <iostream>

#define __WINDOWS_WASAPI__
#define __WINDOWS_DS__
#define __WINDOWS_ASIO__

#define WIDTH (2560/2)
#define HEIGHT (1440/2)

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
		std::cout << "Failed to open an SDL window: " << std::endl << SDL_GetError() << std::endl;
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
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);
	
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
	SDL_GL_SetSwapInterval(1);
#pragma endregion

	Locator &sys = Locator::Get();
	Inputs inputs;
	Time time;

	bool quit = false;
	SDL_Event event;

	GL::ProgRef prog = GL::Programs.Load({"basic.vert", "basic.frag"});

	std::array<float, 18> vertices = {
		// positions         
		1.0f,  1.0f, 0.0f,   
		1.0f, -1.0f, 0.0f,   
		-1.0f, -1.0f, 0.0f,  

		1.0f,  1.0f, 0.0f,   
		-1.0f, -1.0f, 0.0f,  
		-1.0f,  1.0f, 0.0f,  
	};

	std::array<float, 12> uv = {
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
	
		1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 1.0f 
	};

	unsigned int VBO[2], VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);

	glBindVertexArray(VAO);

	GLHelper::SetVBOData(VBO[0], vertices, 3, 0);
	GLHelper::SetVBOData(VBO[1], uv, 2, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindVertexArray(0); 

	GL::Tex2D tex;
	tex.Setup("DS.jpg");

	Camera cam(Vector3(0.f, 0.f, 0.f), Vector3::Left);

	Inputs::State state;

	while(!quit)
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//void Camera::Update(float speed, float lockView, float horizMove, float vertMove, float horizMouse, float vertMouse, float scroll, float deltaTime)
		float horizMove = 0.f, vertMove = 0.f, horizMouse = 0.f, vertMouse = 0.f, scroll = 0.f;
		dVec2 mouseMove;
		sys.Call("Inputs/GetMouseMove", mouseMove);
		horizMouse = mouseMove.x;
		//vertMouse = mouseMove.y;

		sys.Call("Inputs/GetKey", SDL_SCANCODE_A, state);
		horizMove -= (state == Inputs::Held);
		sys.Call("Inputs/GetKey", SDL_SCANCODE_D, state);
		horizMove += (state == Inputs::Held);

		sys.Call("Inputs/GetKey", SDL_SCANCODE_W, state);
		vertMove += (state == Inputs::Held);
		sys.Call("Inputs/GetKey", SDL_SCANCODE_S, state);
		vertMove -= (state == Inputs::Held);
		cam.Update(1.f, false, horizMove, vertMove, horizMouse, vertMouse, scroll, Time::DeltaTime());

		//Math::Mat4 mat;
		//mat = Math::GL::Translate(mat, fVec3(sinf(Time::ProgramTime()) * 1.f,0.f, 0.f));
		//mat = Math::GL::Rotate(mat, Time::ProgramTime() * 0.5f, Vector3(0.f, 0.f, 1.f));
		//mat = Math::GL::Scale(mat, Vector3(tex.Ratio(), 1.f, 1.f));

		Math::Mat4 mat;
		mat = Math::GL::Translate(mat, (fVec3)fVec2(sinf(Time::ProgramTime()) * 1.f, 0.f));
		mat = Math::GL::Rotate(mat, Time::ProgramTime() * 0.5f, Vector3(0.f, 0.f, 1.f));
		mat = Math::GL::Scale(mat, Vector3(tex.Ratio(), 1.f, 1.f));

		prog->Use();
		prog->Set("model", mat);
		//prog->Set("view", Math::GL::LookAt(cam.Pos, cam.Pos + cam.Front, Vector3::Up));
		//prog->Set("projection", Math::GL::Perspective(Math::Deg2Rad * 80.f, ((float)WIDTH / HEIGHT), 0.01f, 1000.f));
		prog->Set("view", Math::Mat4());
		prog->Set("projection", Math::Mat4());

		tex.Bind();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		sys.Call("Update"); 

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
		
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{  sys.Call("Inputs/SetKey", event); } break;
			case SDL_MOUSEMOTION:
			{ sys.Call("Inputs/SetMousePos", event); } break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{ sys.Call("Inputs/SetMouseButton", event); } break;
			case SDL_MOUSEWHEEL:
			{ sys.Call("Inputs/SetMouseWheel", event); } break;
			}
		}

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}