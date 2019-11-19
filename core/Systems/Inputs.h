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
	~Inputs();
	void StartDebug(), StopDebug();

public:
	void setKey(SDL_Event &);
	void setMouseButton(SDL_Event &);
	void setMousePos(SDL_Event );
	void update();

	void getKey(SDL_Scancode code, State &state);
	void getMouseButton(byte button, State &state);
	void getMousePos(dVec2 &v);

	std::array<State, SDL_NUM_SCANCODES> keyStates;
	std::array<State, 5> buttonStates;
	dVec2 mouseChange;
	dVec2 mousePos;
};

class Inputs::Logger
{
private:
	friend Singleton<Logger>;
	friend Inputs;

	Logger(Inputs &inputs) : inputs(inputs)
	{
		tree =
		(
			CNSTR "Inputs", 
			Locator::CmdNode("SetKey", this, &Logger::setKey),
			Locator::CmdNode("SetMouseButton", this, &Logger::setMouseButton),
			Locator::CmdNode("SetMousePos", this, &Logger::setMousePos),
			CNEND
		);
	}

	void start(), stop();

	void setKey(SDL_Event &event);
	void setMouseButton(SDL_Event &event);
	void setMousePos(SDL_Event &event);

private:
	Locator::CmdNode tree;
	Inputs &inputs;
};