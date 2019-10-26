#pragma once

#include "Systems/Locator.h"
#include "Vec.h"

#include "SDL.h"

class Inputs
{
	enum State
	{
		Pressed,
		Held,
		Released,
		Free
	};

	enum NewState
	{
		WasPressed = SDL_PRESSED,
		WasReleased = SDL_RELEASED
	};

	void test() {}

public:
	Inputs()
	{ 
		for(size_t i = 0; i < SDL_NUM_SCANCODES; i++) { keyStates[i] = Free; }
		for(size_t i = 0; i < 5; i++) { buttonStates[i] = Free; }

		CNTREE
		(
			CNSTR Input, 
				Locator::CmdNode(SetKey,      this, &Inputs::keyPress),
				Locator::CmdNode(SetButton,   this, &Inputs::mousePress),
				Locator::CmdNode(SetMousePos, this, &Inputs::mouseMove),
				//CmdNode(GetKey,      this, &Inputs::getKeyUp),
				//CmdNode(GetButton,   this, &Inputs::isButtonUp),
				//CmdNode(GetMousePos, this, &Inputs::getMousePos),
			CNEND
		);
	}

private:
	void keyPress(SDL_Event &);
	void mousePress(SDL_Event &);
	void mouseMove(SDL_Event );
	void update();

	State getKey(SDL_Scancode code), getButton(byte button);
	dVec2 getMousePos();
	bool getKeyUp(void *), getKeyDown(void *), isButtonUp(void *), isButtonDown(void *);

	State keyStates[SDL_NUM_SCANCODES];
	State buttonStates[5];
	dVec2 mouseChange;
	dVec2 mousePos;
};