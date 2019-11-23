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

private:
	void ignoreInputs(bool ignore);

	void setKey(SDL_Event &);
	void setMouseButton(SDL_Event &);
	void setMousePos(SDL_Event );
	void update();

	void getKey(SDL_Scancode code, State &state);
	void getMouseButton(byte button, State &state) ;
	void getMousePos(dVec2 &v) ;

	std::array<State, SDL_NUM_SCANCODES> keyStates;
	std::array<State, 5> buttonStates;
	dVec2 mouseChange;
	dVec2 mousePos;
	bool isIgnored;

	Locator::CmdNode tree;
};

class Inputs::Logger
{
private:
	friend Inputs;

	Logger(Inputs &inputs);

	void start(), stop();

	void setKey(SDL_Event &event);
	void setMouseButton(SDL_Event &event);
	void setMousePos(SDL_Event &event);

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

	void whileRecording(SDL_Event &event);
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