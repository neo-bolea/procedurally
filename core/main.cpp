#define GLEW_STATIC
#define SDL_MAIN_HANDLED
#include "pch.h"
#include "framework.h"
#include "Core.h"

#include "IOUtil.h"
#include "Systems/Inputs.h"
#include "Hash.h"
#include "Mat.h"
#include "StringUtils.h"
#include "Vec.h"
#include "Watch.h"

#include "GL/glew.h"
#include "SDL.h"

#include <iostream>

#define __WINDOWS_WASAPI__
#define __WINDOWS_DS__
#define __WINDOWS_ASIO__

#define WIDTH (1920/2)
#define HEIGHT (1080/2)

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

	SDL_GL_SetSwapInterval(0);

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
#pragma endregion

	Locator &sys = Locator::Get();

	bool quit = false;
	SDL_Event event;
	event.key.keysym.scancode = (SDL_Scancode)0;

	Inputs inputs;

	Watch watch(Watch::ms);
	watch.Start();
	for(size_t i = 0; i < 1'000'000; i++)
	{
		sys.Call({ CmdKey::Input, CmdKey::GetMousePos }, 1);
	}
	watch.Stop();
	std::cout << watch.sTime() << std::endl;

	while(!quit)
	{
		glClearColor(0.f, 1.f, 0.7f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		sys.Call({CmdKey::Update});

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{  sys.Call({ CmdKey::Input, CmdKey::SetKey }, event); } break;

			case SDL_MOUSEMOTION:
			{ sys.Call({ CmdKey::Input, CmdKey::SetMousePos }, event); } break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEWHEEL:
			{ sys.Call({ CmdKey::Input, CmdKey::SetButton }, event); } break;
			}
		}

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}