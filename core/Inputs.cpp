#include "Systems/Inputs.h"

#include <iostream>

void Inputs::keyPress(SDL_Event &sdlEvent)
{
	SDL_KeyboardEvent key = sdlEvent.key;
	State state;
	getKey(key.keysym.scancode, state);

	if(key.state == WasPressed)
	{
		if(state == Free || state == Released) { state = Pressed; }
	}
	else if(key.state == WasReleased)
	{ 
		if(state == Pressed || state == Held) { state = Released; }
	}
	sdlEvent.button.button = 100;
	//std::cout<< key.keysym.scancode << std::endl;
}

void Inputs::mousePress(SDL_Event &sdlEvent)
{
	SDL_MouseButtonEvent button = sdlEvent.button;
	State &state = buttonStates[button.button];

	if(button.state == WasPressed)
	{
		if(state == Free || state == Released) { state = Pressed; }
	}
	else if(button.state == WasReleased)
	{ 
		if(state == Pressed || state == Held) { state = Released; }
	}

	//std::cout << (int)button.button << std::endl;
}

void Inputs::mouseMove(SDL_Event sdlEvent)
{
	mouseChange.x += sdlEvent.motion.xrel;
	mouseChange.y += sdlEvent.motion.yrel;
	mousePos.x = sdlEvent.motion.x;
	mousePos.y = sdlEvent.motion.y;

	//std::cout << mousePos.ToString() << std::endl;
}

void Inputs::update()
{
	for(size_t i = 0; i < buttonStates.size(); i++)
	{
		State &button = buttonStates[i];
		if(button == Released) { button = Free; }
		else if(button == Pressed) { button = Held; }
	}

	for(size_t i = 0; i < keyStates.size(); i++)
	{
		State &key = keyStates[i];
		if(key == Released) { key = Free; }
		else if(key == Pressed) { key = Held; }
	}

	mouseChange = dVec2(0.0); 
}

void Inputs::getKey(SDL_Scancode code, State &state)
{ state = keyStates[code]; }

void Inputs::getButton(byte button, State &state)
{ state = buttonStates[button]; }

dVec2 Inputs::getMousePos()
{ return mousePos; }