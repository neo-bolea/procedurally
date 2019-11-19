#include "Systems/Inputs.h"

#include <iostream>

Inputs::Inputs()
{ 
	for(size_t i = 0; i < keyStates.size(); i++) { keyStates[i] = Free; }
	for(size_t i = 0; i < buttonStates.size(); i++) { buttonStates[i] = Free; }

	CNTREE
	(
		CNSTR "Inputs", 
		Locator::CmdNode("SetKey", this, &Inputs::keyPress),
		Locator::CmdNode("SetButton", this, &Inputs::mousePress),
		Locator::CmdNode("SetMousePos", this, &Inputs::mouseMove),
		Locator::CmdNode("GetButton", this, &Inputs::getButton),
		CNEND
	);		

	CNTREE
	(
		Locator::CmdNode("Update", this, &Inputs::update)
	);
}

void Inputs::StartDebug() { Logger::Get().Start(); }
void Inputs::StopDebug() { Logger::Get().Stop(); }

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
}

void Inputs::mouseMove(SDL_Event sdlEvent)
{
	mouseChange.x += sdlEvent.motion.xrel;
	mouseChange.y += sdlEvent.motion.yrel;
	mousePos.x = sdlEvent.motion.x;
	mousePos.y = sdlEvent.motion.y;
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


void Inputs::Logger::setKey(SDL_Event &event)
{ std::cout << "Key changed: " << event.key.keysym.scancode << std::endl; }
void Inputs::Logger::mousePress(SDL_Event &event)
{ std::cout << "Button changed: " << (int)event.button.button << std::endl; }
void Inputs::Logger::mouseMove(SDL_Event &event)
{ std::cout << "Mouse moved: (" << event.motion.x << ", " << event.motion.y << ")" << std::endl; }