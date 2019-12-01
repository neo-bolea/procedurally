#pragma once

#include "Locator.h"
#include "Time.h"
#include "Vec.h"

#include "SDL.h"

#include <any>

class Inputs
{
private:
	class Logger;
	class Recorder;
	Logger *logger;
	Recorder *recorder;

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
	void StartRecording(), StopRecording();
	void StartReplaying(), StopReplaying();

	State GetKey(SDL_Scancode code);
	State GetMouseButton(byte button);
	dVec2 GetMousePos();
	dVec2 GetMouseMove();
	dVec2 GetMouseWheel();

public:
	void ignoreInputs(bool ignore);

	void setKey(SDL_Event &);
	void setMouseButton(SDL_Event &);
	void setMousePos(SDL_Event &);
	void setMouseWheel(SDL_Event &);
	void update();

	void GetKey_(SDL_Scancode code, State &state);
	void GetMouseButton_(byte button, State &state);
	void GetMousePos_(dVec2 &v);
	void GetMouseMove_(dVec2 &v);
	void GetMouseWheel_(dVec2 &v);

	std::array<State, SDL_NUM_SCANCODES> keyStates;
	std::array<State, 5> buttonStates;
	dVec2 mouseMove;
	dVec2 mousePos;
	dVec2 mouseWheelMove;
	bool isIgnored;

	Locator::CmdNode tree;
};

class Inputs::Logger
{
private:
	friend Inputs;

	Logger(Inputs &inputs);

	void start(), stop();

	void setKey(SDL_Event &);
	void setMouseButton(SDL_Event &);
	void setMousePos(SDL_Event &);
	void setMouseWheel(SDL_Event &);

private:
	Locator::CmdNode tree;
	Inputs &inputs;
};

class Inputs::Recorder
{
private:
	friend Inputs;

	Recorder(Inputs &inputs);

	void startRecording(), stopRecording();
	void startReplaying(), stopReplaying();

	void whileRecording(SDL_Event &);
	void whileReplaying();

	void simulateNextInput();

private:
	struct InputInfo { Time::TimePoint Time; SDL_EventType Type; std::any Value; };
	std::vector<InputInfo> recordedInputs;

	bool isRecording = false, isReplaying = false;
	Time::TimePoint actionStartTime;
	int replayCurrentInput = -1;

	Locator::CmdNode recordTree;
	Locator::CmdNode replayTree;
	Inputs &inputs;
};