#include "Systems/Inputs.h"

#include <iostream>

Inputs::Inputs() : logger(new Logger(*this))
{ 
	for(size_t i = 0; i < keyStates.size(); i++) { keyStates[i] = Free; }
	for(size_t i = 0; i < buttonStates.size(); i++) { buttonStates[i] = Free; }

	CNTREE
	(
		CNSTR "Inputs", 
		Locator::CmdNode("SetKey", this, &Inputs::setKey),
		Locator::CmdNode("GetKey", this, &Inputs::getKey),
		Locator::CmdNode("SetMouseButton", this, &Inputs::setMouseButton),
		Locator::CmdNode("GetMouseButton", this, &Inputs::getMouseButton),
		Locator::CmdNode("SetMousePos", this, &Inputs::setMousePos),
		Locator::CmdNode("GetMousePos", this, &Inputs::getMousePos),
		CNEND
	);		

	CNTREE
	(
		Locator::CmdNode("Update", this, &Inputs::update)
	);
}

Inputs::~Inputs() { delete logger; }

void Inputs::StartDebug() { logger->start(); }
void Inputs::StopDebug() { logger->stop(); }

void Inputs::setKey(SDL_Event &sdlEvent)
{
	SDL_KeyboardEvent key = sdlEvent.key;
	State &state = keyStates[key.keysym.scancode];

	if(key.state == WasPressed)
	{
		if(state == Free || state == Released) { state = Pressed; }
	}
	else if(key.state == WasReleased)
	{ 
		if(state == Pressed || state == Held) { state = Released; }
	}
}

void Inputs::setMouseButton(SDL_Event &sdlEvent)
{
	SDL_MouseButtonEvent button = sdlEvent.button;
	State &state = buttonStates[button.button - 1];

	if(button.state == WasPressed)
	{
		if(state == Free || state == Released) { state = Pressed; }
	}
	else if(button.state == WasReleased)
	{ 
		if(state == Pressed || state == Held) { state = Released; }
	}
}

void Inputs::setMousePos(SDL_Event sdlEvent)
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
{ 
	state = keyStates[code];
}

void Inputs::getMouseButton(byte button, State &state)
{ state = buttonStates[button]; }

void Inputs::getMousePos(dVec2 &v)
{ v = mousePos; }


void Inputs::Logger::start()
{ Locator::Get().Add(tree); }

void Inputs::Logger::stop()
{ Locator::Get().Remove(tree); }

void Inputs::Logger::setKey(SDL_Event &event)
{ std::cout << "Key changed: " << event.key.keysym.scancode << std::endl; }
void Inputs::Logger::setMouseButton(SDL_Event &event)
{ std::cout << "Button changed: " << (int)event.button.button << std::endl; }
void Inputs::Logger::setMousePos(SDL_Event &event)
{ std::cout << "Mouse moved: (" << event.motion.x << ", " << event.motion.y << ")" << std::endl; }