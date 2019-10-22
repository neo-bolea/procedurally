#include "Systems/Inputs.h"

void Inputs::keyPress(SDL_Event &sdlEvent)
{
	SDL_KeyboardEvent key = sdlEvent.key;
	State state = getKey(key.keysym.scancode);

	if(key.state == WasPressed)
	{
		if(state == Free || state == Released) { state = Pressed; }
	}
	else if(key.state == WasReleased)
	{ 
		if(state == Pressed || state == Held) { state = Released; }
	}

	std::cout << key.keysym.scancode << std::endl;
}

void Inputs::mousePress(SDL_Event &sdlEvent)
{
	SDL_MouseButtonEvent button = sdlEvent.button;
	State state = getButton(button.button);

	if(button.state == WasPressed)
	{
		if(state == Free || state == Released) { state = Pressed; }
	}
	else if(button.state == WasReleased)
	{ 
		if(state == Pressed || state == Held) { state = Released; }
	}
}

void Inputs::mouseMove(SDL_Event sdlEvent)
{
	mouseChange.x += sdlEvent.motion.xrel;
	mouseChange.y += sdlEvent.motion.yrel;
	mousePos.x = sdlEvent.motion.x;
	mousePos.y = sdlEvent.motion.y;
	std::cout << mousePos.ToString() << std::endl;
}

//TODO: Add SYSTEMS_FUNC_START with no arguments
void Inputs::update()
{
	mouseChange = dVec2(0.0); 
}

Inputs::State Inputs::getKey(SDL_Scancode code)
{ return keyStates[code]; }

Inputs::State Inputs::getButton(byte button)
{ return buttonStates[button - 1]; }

dVec2 Inputs::getMousePos()
{ return mousePos; }

bool Inputs::getKeyUp(void *args) 
{ return keyStates[(size_t)args] == Free || keyStates[(size_t)args] == Released; }

bool Inputs::getKeyDown(void *args)
{ return !getKeyUp(args); }

bool Inputs::isButtonUp(void *args)
{ return buttonStates[(size_t)args] == Free || buttonStates[(size_t)args] == Released; }

bool Inputs::isButtonDown(void *args)
{ return !isButtonUp(args); }