#pragma once

#include "Locator.h"
#include "Vec.h"

#include "SDL.h"

class Inputs
{
private:
	class Logger;
	Logger *logger;

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

	Inputs();
	void StartDebug(), StopDebug();

public:
	void keyPress(SDL_Event &);
	void mousePress(SDL_Event &);
	void mouseMove(SDL_Event );
	void update();

	void getKey(SDL_Scancode code, State &state);
	void getButton(byte button, State &state);
	dVec2 getMousePos();

	std::array<State, SDL_NUM_SCANCODES> keyStates;
	std::array<State, 6> buttonStates;
	dVec2 mouseChange;
	dVec2 mousePos;
};

static class Inputs::Logger : public Singleton<Logger>
{
public:
	Locator::CmdNode *tree;

	void Start()
	{ 
		tree = new Locator::CmdNode
		(
			CNSTR "Inputs", 
			Locator::CmdNode("SetKey", this, &Logger::setKey),
			Locator::CmdNode("SetButton", this, &Logger::mousePress),
			Locator::CmdNode("SetMousePos", this, &Logger::mouseMove),
			CNEND
		);		

		Locator::Get().Add(*tree);
	}

	void Stop()
	{ 
		Locator::Get().Remove(*tree); 
		delete tree;
	}

private:
	friend Singleton<Logger>;
	Logger() {}

	void setKey(SDL_Event &event);
	void mousePress(SDL_Event &event);
	void mouseMove(SDL_Event &event);
};