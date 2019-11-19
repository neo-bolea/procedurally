#pragma once

#include "Locator.h"
#include "Vec.h"

#include "SDL.h"

class Inputs
{
private:
	enum NewState
	{
		WasPressed = SDL_PRESSED,
		WasReleased = SDL_RELEASED
	};

public:
	enum State
	{
		Undefined = 0,
		Pressed,
		Held,
		Released,
		Free
	};

	Inputs()
	{ 
		for(size_t i = 0; i < SDL_NUM_SCANCODES; i++) { keyStates[i] = Free; }
		for(size_t i = 0; i < 5; i++) { buttonStates[i] = Free; }

		//CNTREE
		//(
		//	CNSTR Input, 
		//		Locator::CmdNode(SetKey,      this, &Inputs::keyPress),
		//		Locator::CmdNode(SetButton,   this, &Inputs::mousePress),
		//		Locator::CmdNode(SetMousePos, this, &Inputs::mouseMove),
		//		Locator::CmdNode(GetButton, this, &Inputs::getButton),
		//	CNEND
		//);		
		//
		//CNTREE
		//(
		//	Locator::CmdNode(Update, this, &Inputs::update)
		//);
	}

public:
	void keyPress(SDL_Event &);
	void mousePress(SDL_Event &);
	void mouseMove(SDL_Event );
	void update();

	void getKey(SDL_Scancode code, State &state);
	void getButton(byte button, State &state);
	dVec2 getMousePos();

	std::array<State, SDL_NUM_SCANCODES> keyStates;
	std::array<State, 5> buttonStates;
	dVec2 mouseChange;
	dVec2 mousePos;
};