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

//function Algorithm(bufferSize)
//{
//	//FInd number of drops
//	D = (simulationArea * rainRate * bufferSize) / (dropVolume * sampleRate * 3.6e6);
//
//	//Prepare all drops
//	repeat with d = 1 to D
//	{
//		//rand = a floating point random number between 0 and 1, uniformly distributed
//		dropArray(d).direction = rand * 2 * p;
//		dropArray(d).distance = distanceMin + v(rand) * (distanceMax - distanceMin);
//		dropArray(d).sourceRadius = sourceRadiusMin + rand * (sourceRadiusMax - sourceRadiusMin);
//		dropArray(d).timeShort = rand * bufferSize / D * ceiling(D);
//		dropArray(d).timeLong = timeShort + (Rl - Rs) / speedOfSound * sampleRate;
//		dropArray(d).timeHit = timeShort - Rs / speedOfSound * sampleRate;
//	}
//
//	//Sort dropArray
//	quickSort(dropArray, timeShort);
//
//	//Initialize firstDrop variable
//	firstDrop = dropArray(1);
//
//	//Calculate the resulting pressure
//	repeat for each sample in buffer
//	{
//		//Initiate a sample frame, a collection of sample values, one for each channel
//		thisFrame = array of N values, where N = number of channels
//
//		repeat with each drop in dropArray beginning with firstDrop
//		{
//			if thisDrop.timeShort > thisSample then
//			//i.e. thisDrop begins beyond thisSample
//			{
//				//Bounds the repeat loop from right
//				exit this loop and proceed to next sample
//			}
//			or if thisDrop.timeShort <= thisSample and thisDrop.timeLong >= thisSample
//			//i.e. thisDrop encapsulates thisSample
//			{
//				//Use the model to find the pressure value for this drop at this time
//				dropContribution = calculateSourcePressure(thisDrop, thisSample);
//				
//				//Weight and add the pressure to this sample frame
//				thisFrame = thisFrame + distributeSound(thisDrop, dropContribution);
//
//				if thisDrop is first encountered drop for thisSample
//				{
//					//Bounds the repeat loop from left
//					firstDrop = thisDrop;
//				}
//			}
//		}
//	}
//
//	return all frames produced for this sample period;
//}
//
//float calculateSoundPressure(thisDrop, thisSample)
//{
//	//Remeber that thisDrop.timeShort <= thisSample <= thisDrop.timeLong
//	//hence t will be the local time from timeHit(t = 0) to a time within sound
//	t = thisSample - thisDrop.timeHit;
//
//	//t = 0 when drop hits surface
//	//The model is only valid for t = timeShort to timeLong
//	samplePressure = model(thisDrop, t);
//
//	return samplePressure;
//}
//
//function distributeSound(thisDrop, dropContribution)
//{
//	//N = number of channels
//	if N = 1
//	//i.e. mono sound output
//	{
//		return dropContribution;
//		//ends this function call
//	}
//	else if N > 1
//	//I.e. multi channel output
//	{
//		//Initiate array of sorted speaker directions
//		speakerArray = angle1, angle2 ...;
//		//0 is straight forward in simulation space
//		//ex. stereo: 90, 270
//
//		//Repeat through all area segments to find the area where this drop fell
//		repeat with index i = 1 to N
//		{
//			//Calculate the next speaker index with modulus
//			j = mod(i + 1, N);
//			//ex. if i = 2 and N = 2, j = 1
//
//			//Find the bounding section between two speakers
//			firstAngle = speakerArray(i);
//			lastAngle = speakerArray(j);
//			a = thisDrop.direction;
//
//			if lastAngle > firstAngle
//			{
//				if a >= firstAngle and a < lastAngle
//				//i.e. drop angle is encapsulated by this section
//				{
//					alpha = (lastAngle - a) / (lastAngle - firstAngle);
//					exit repeat;
//				}
//			}
//			else
//			{
//				if a >= firstAngle
//				{
//					alpha = 1 - (a - firstAngle) / (firstAngle - lastAngle);
//					exit repeat;
//				}
//				else if a < lastAngle
//				{
//					alpha = (lastAngle - a) / (firstAngle - lastAngle);
//					exit repeat;
//				}
//			}
//		}
//	}
//
//	//Frame = a collection of sample values, one for each speaker
//	frame = array of N sample values;
//	frame(i) = alpha * dropContribution;
//	frame(j) = (alpha - 1) * dropContribution;
//
//	return frame;
//}
//
//function model(thisDrop, t)
//{
//	p(0, 0, H, t) = 
//	{
//		0 if t < Rs/c,
//		(p0*c/pi)*A*(vT)*cos^-1*(
//		0 if t > Rl/c,
//	}
//}

#define WIDTH (1920/2)
#define HEIGHT (1080/2)

using namespace CmdKeys;

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

	//	sys.Call({CmdKey::Update});

		//TODO: Create wrapper for easier return values.
		//TODO: Add handles for typical inputs?
		//sys.Call({CmdKey::Input, CmdKey::GetButton}, 0)
		//if()
		//{
		//
		//}
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