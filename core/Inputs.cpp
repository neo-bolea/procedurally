#include "Systems/Inputs.h"

#include "Debug.h"
#include "MathExt.h"

#include <iostream>

Inputs::Inputs() : logger(new Logger(*this)), recorder(new Recorder(*this))
{ 
	for(size_t i = 0; i < keyStates.size(); i++) { keyStates[i] = Free; }
	for(size_t i = 0; i < buttonStates.size(); i++) { buttonStates[i] = Free; }

	tree =
	(
		CNSTART "Inputs", 
		Locator::CmdNode("SetKey", this, &Inputs::setKey),
		Locator::CmdNode("GetKey", this, &Inputs::GetKey_),
		Locator::CmdNode("SetMouseButton", this, &Inputs::setMouseButton),
		Locator::CmdNode("GetMouseButton", this, &Inputs::GetMouseButton_),
		Locator::CmdNode("SetMousePos", this, &Inputs::setMousePos),
		Locator::CmdNode("GetMousePos", this, &Inputs::GetMousePos_),
		Locator::CmdNode("GetMouseMove", this, &Inputs::GetMouseMove_),
		Locator::CmdNode("SetMouseWheel", this, &Inputs::setMouseWheel),
		Locator::CmdNode("GetMouseWheel", this, &Inputs::GetMouseWheel_),
		CNEND
	);

	Locator::Add(tree);		

	Locator::Add(Locator::CmdNode{ "Update", this, &Inputs::update });

	isIgnored = false;
}

Inputs::~Inputs() { delete logger; delete recorder; }

void Inputs::StartDebug() { logger->start(); }
void Inputs::StopDebug() { logger->stop(); }

void Inputs::StartRecording() { recorder->startRecording(); }
void Inputs::StopRecording() { recorder->stopRecording(); }

void Inputs::StartReplaying() { recorder->startReplaying(); }
void Inputs::StopReplaying() { recorder->stopReplaying(); }

void Inputs::ignoreInputs(bool ignore)
{
	if(ignore && !isIgnored)
	{
		Locator::Remove(tree);
		isIgnored = true;
	}

	if(!ignore && isIgnored)
	{
		Locator::Add(tree);
		isIgnored = false;
	}
}

Inputs::State Inputs::GetKey(Key code)
{
	State state;
	Locator::Call("Inputs/GetKey", code, state);
	return state;
}
Inputs::State Inputs::GetMouseButton(Button button)
{
	State state;
	Locator::Call("Inputs/GetMouseButton", button, state);
	return state;
}
dVec2 Inputs::GetMousePos()
{
	dVec2 v;
	Locator::Call("Inputs/GetMousePos", v);
	return v;
}
dVec2 Inputs::GetMouseMove()
{
	dVec2 v;
	Locator::Call("Inputs/GetMouseMove", v);
	return v;
}
dVec2 Inputs::GetMouseWheel()
{
	dVec2 v;
	Locator::Call("Inputs/GetMouseWheel", v);
	return v;
}

bool Inputs::IsKeyPressed(Key code) { return GetKey(code) == Pressed; }
bool Inputs::IsKeyReleased(Key code) { return GetKey(code) == Released; }
bool Inputs::IsKeyDown(Key code) 
{
	State state = GetKey(code);
	return state == Pressed || state == Held;
}
bool Inputs::IsKeyUp(Key code) { return !IsKeyDown(code); }

bool Inputs::IsMouseButtonPressed(Button button) { return GetMouseButton(button) == Pressed; }
bool Inputs::IsMouseButtonReleased(Button button) { return GetMouseButton(button) == Released; }
bool Inputs::IsMouseButtonDown(Button button)
{
	State state = GetMouseButton(button);
	return state == Pressed || state == Held;
}
bool Inputs::IsMouseButtonUp(Button button) { return !IsMouseButtonDown(button); }


void Inputs::AddAxis(const std::string &name, AnyInput positive, AnyInput negative)
{ axes.insert({ hash(name), { positive, negative, 0 } }); }

double Inputs::GetAxis(const std::string &name, AxisValueMode mode)
{ 
	auto &info = getAxisInfo(name);
	if (!info.has_value()) { return 0.0; }

	if (mode == Raw) { return info.value().value; }
	else if(mode == Smooth) { return info.value().smoothValue; }

	Debug::Log("Unknown input axis mode has been requested.", Debug::Error, { "Inputs" });
	return 0.0;
}


void Inputs::setKey(SDL_Event &event)
{
	SDL_KeyboardEvent key = event.key;
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

void Inputs::setMouseButton(SDL_Event &event)
{
	SDL_MouseButtonEvent button = event.button;
	if (button.button <= 0)
	{
		Debug::Log("Pressed button was outside the range \
			(1, " + std::to_string(buttonStates.size()) + "):" + std::to_string(button.button), Debug::Info);
		return;
	}
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

void Inputs::setMousePos(SDL_Event &sdlEvent)
{
	mouseMove.x += sdlEvent.motion.xrel;
	mouseMove.y += sdlEvent.motion.yrel;
	mousePos.x = sdlEvent.motion.x;
	mousePos.y = sdlEvent.motion.y;
}

void Inputs::setMouseWheel(SDL_Event &event)
{
	mouseWheelMove.x = event.wheel.x;
	mouseWheelMove.y = event.wheel.y;
}

void Inputs::update()
{
	// Update buttons
	for(size_t i = 0; i < buttonStates.size(); i++)
	{
		State &button = buttonStates[i];
		if(button == Released) { button = Free; }
		else if(button == Pressed) { button = Held; }
	}

	// Update keys
	for(size_t i = 0; i < keyStates.size(); i++)
	{
		State &key = keyStates[i];
		if(key == Released) { key = Free; }
		else if(key == Pressed) { key = Held; }
	}

	// Update axes
	for(auto &axisPair : axes)
	{
		auto &axis = axisPair.second;
		double negVal, posVal;

		if(axis.positive.IsKey()) 
		{ posVal = IsKeyDown(std::get<Key>(axis.positive.value)); }
		else { posVal = IsMouseButtonDown(std::get<Button>(axis.positive.value)); }

		if(axis.negative.IsKey())
		{ negVal = IsKeyDown(std::get<Key>(axis.negative.value)); }
		else { negVal = IsMouseButtonDown(std::get<Button>(axis.negative.value)); }

		axis.value = posVal - negVal;
		axis.smoothValue = Math::TimedExpEase(axis.smoothValue, axis.value, smoothAxisSharpness, Time::DeltaTime());
	}

	mouseMove = dVec2(0.0); 
}

void Inputs::GetKey_(Key code, State &state)
{ state = keyStates[static_cast<int>(code)]; }

void Inputs::GetMouseButton_(Button button, State &state)
{ state = buttonStates[button]; }

void Inputs::GetMousePos_(dVec2 &v)
{ v = mousePos; }

void Inputs::GetMouseMove_(dVec2 &v)
{ v = mouseMove; }

void Inputs::GetMouseWheel_(dVec2 &v)
{ v = mouseWheelMove; }


std::optional<Inputs::AxisInfo> Inputs::getAxisInfo(const std::string &name)
{
	auto axis = axes.find(hash(name));
	if (axis == axes.end())
	{
		Debug::Log("The requested input axis was not found.", Debug::Error, { "Inputs" });
		return std::nullopt;
	}
	return axis->second;
}


Inputs::stringHash Inputs::hash(const std::string &str)
{ return CRC32::Get(str.data(), str.size()); }



Inputs::Logger::Logger(Inputs &inputs) : inputs(inputs)
{
	tree =
	(
		CNSTART "Inputs", 
		Locator::CmdNode("SetKey", this, &Logger::setKey),
		Locator::CmdNode("SetMouseButton", this, &Logger::setMouseButton),
		Locator::CmdNode("SetMousePos", this, &Logger::setMousePos),
		Locator::CmdNode("SetMouseWheel", this, &Logger::setMouseWheel),
		CNEND
	);
}

void Inputs::Logger::start()
{ Locator::Add(tree); }

void Inputs::Logger::stop()
{ Locator::Remove(tree); }

void Inputs::Logger::setKey(SDL_Event &event)
{ std::cout << "Key changed: " << event.key.keysym.scancode << std::endl; }
void Inputs::Logger::setMouseButton(SDL_Event &event)
{ std::cout << "Button changed: " << (int)event.button.button << std::endl; }
void Inputs::Logger::setMousePos(SDL_Event &event)
{ std::cout << "Mouse moved: (" << event.motion.x << ", " << event.motion.y << ")" << std::endl; }
void Inputs::Logger::setMouseWheel(SDL_Event &event)
{ std::cout << "Mouse wheel moved: (" << event.wheel.x << ", " << event.wheel.y << ")" << std::endl; }

Inputs::Recorder::Recorder(Inputs &inputs) : inputs(inputs)
{
	recordTree =
	(
		CNSTART "Inputs", 
		Locator::CmdNode("SetKey", this, &Recorder::whileRecording),
		Locator::CmdNode("SetMouseButton", this, &Recorder::whileRecording),
		Locator::CmdNode("SetMousePos", this, &Recorder::whileRecording),
		Locator::CmdNode("SetMouseWheel", this, &Recorder::whileRecording),
		CNEND
	);

	replayTree = Locator::CmdNode("Update", this, &Recorder::whileReplaying);
}

void Inputs::Recorder::startRecording()
{ 
	if(isRecording) { stopRecording(); }

	Locator::Add(recordTree); 

	actionStartTime = Time::ProgramTime();
	isRecording = true;
}

void Inputs::Recorder::stopRecording()
{ 
	if(isReplaying) { stopReplaying(); }

	Locator::Remove(recordTree);
	isRecording = false;
}

void Inputs::Recorder::startReplaying()
{
	inputs.ignoreInputs(true);

	if(isReplaying) { stopReplaying(); }
	if(isRecording) { stopRecording(); }

	Locator::Add(replayTree); 

	actionStartTime = Time::ProgramTime();
	replayCurrentInput = 0;
	isReplaying = true;
}

void Inputs::Recorder::stopReplaying()
{
	inputs.ignoreInputs(false);

	if(isRecording)
	{ stopRecording(); }

	Locator::Remove(replayTree);
	isReplaying = false;
}

void Inputs::Recorder::whileRecording(SDL_Event &event)
{ 
	InputInfo info;
	info.Time = Time::ProgramTime() - actionStartTime;
	info.Type = static_cast<SDL_EventType>(event.type);

	switch(event.type)
	{
	case SDL_KEYUP:
	case SDL_KEYDOWN:
	{ info.Value = event.key.keysym.scancode; } break;

	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEBUTTONDOWN:
	{ info.Value = event.button.button; } break;

	case SDL_MOUSEMOTION:
	{ info.Value = dVec2(event.motion.x, event.motion.y); } break;

	case SDL_MOUSEWHEEL:
	{ info.Value = dVec2(event.wheel.x, event.wheel.y); } break;
	}
	recordedInputs.push_back(info);
}

void Inputs::Recorder::whileReplaying()
{ 
	if(replayCurrentInput >= recordedInputs.size())
	{
		stopReplaying();
		return;
	}

	while(Time::ProgramTime() - actionStartTime > recordedInputs[replayCurrentInput].Time)
	{
		simulateNextInput();

		replayCurrentInput++;
		if(replayCurrentInput >= recordedInputs.size())
		{
			stopReplaying();
			return;
		}
	}
}

void Inputs::Recorder::simulateNextInput()
{
	InputInfo nextInput = recordedInputs[replayCurrentInput];
	SDL_Event event = {};
	event.type = nextInput.Type;
	
	switch(nextInput.Type)
	{
	case SDL_KEYUP:
	case SDL_KEYDOWN:
	{ event.key.keysym.sym = static_cast<SDL_Keycode>(std::any_cast<Key>(nextInput.Value)); } break;
	
	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEBUTTONDOWN:
	{ event.button.button = std::any_cast<Uint8>(nextInput.Value); } break;
	
	case SDL_MOUSEMOTION:
	{ 
		event.motion.x = static_cast<Sint32>(std::any_cast<dVec2>(nextInput.Value).x);
		event.motion.y = static_cast<Sint32>(std::any_cast<dVec2>(nextInput.Value).y);
	} break;
	
	case SDL_MOUSEWHEEL:
	{
		event.wheel.x = static_cast<Sint32>(std::any_cast<dVec2>(nextInput.Value).x);
		event.wheel.y = static_cast<Sint32>(std::any_cast<dVec2>(nextInput.Value).y);
	} break;
	}
	
	SDL_PushEvent(&event);
} 