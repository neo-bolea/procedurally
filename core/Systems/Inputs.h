#pragma once

#include "Locator.h"
#include "Time.h"
#include "Vec.h"

#include "SDL.h"

#include <any>
#include <optional>
#include <variant>

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
	struct AnyInput;

	enum State
	{
		Undefined = 0,
		Pressed,
		Held,
		Released,
		Free
	};

	enum AxisValueMode { Raw, Smooth };

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

	bool IsKeyPressed(SDL_Scancode code);
	bool IsKeyReleased(SDL_Scancode code);
	bool IsKeyDown(SDL_Scancode code);
	bool IsKeyUp(SDL_Scancode code);
		 
	bool IsMouseButtonPressed(byte button);
	bool IsMouseButtonReleased(byte button);
	bool IsMouseButtonDown(byte button);
	bool IsMouseButtonUp(byte button);

	void AddAxis(const std::string &name, AnyInput positive, AnyInput negative);
	double GetAxis(const std::string &name, AxisValueMode mode = Raw);

private:
	using stringHash = uint;
	struct AxisInfo;

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


	std::optional<Inputs::AxisInfo> getAxisInfo(const std::string &str);
	stringHash hash(const std::string &str);

	std::array<State, SDL_NUM_SCANCODES> keyStates;
	std::array<State, 5> buttonStates;
	dVec2 mouseMove;
	dVec2 mousePos;
	dVec2 mouseWheelMove;
	bool isIgnored;

	Locator::CmdNode tree;

	std::unordered_map<stringHash, AxisInfo> axes;
	const double smoothAxisSharpness = 0.99875;
};

struct Inputs::AnyInput
{
	using ValueType = std::variant<SDL_Scancode, byte>;

	AnyInput() : value(0) {}
	AnyInput(ValueType value) : value(value) {}
	AnyInput(SDL_Scancode value) : value(value) {}
	AnyInput(byte value) : value(value) {}

	bool IsKey() { return value.index() == 0; }

	ValueType value;
};

struct Inputs::AxisInfo
{
	AnyInput positive, negative;
	double value, smoothValue;
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