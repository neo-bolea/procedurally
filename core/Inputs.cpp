#include "Systems/Inputs.h"

#include <iostream>

Inputs::Inputs() : logger(new Logger(*this)), recorder(new Recorder(*this))
{ 
	for(size_t i = 0; i < keyStates.size(); i++) { keyStates[i] = Free; }
	for(size_t i = 0; i < buttonStates.size(); i++) { buttonStates[i] = Free; }

	tree =
		(
			CNSTART "Inputs", 
			Locator::CmdNode("SetKey", this, &Inputs::setKey),
			Locator::CmdNode("GetKey", this, &Inputs::getKey),
			Locator::CmdNode("SetMouseButton", this, &Inputs::setMouseButton),
			Locator::CmdNode("GetMouseButton", this, &Inputs::getMouseButton),
			Locator::CmdNode("SetMousePos", this, &Inputs::setMousePos),
			Locator::CmdNode("GetMousePos", this, &Inputs::getMousePos),
			Locator::CmdNode("GetMouseMove", this, &Inputs::getMouseMove),
			Locator::CmdNode("SetMouseWheel", this, &Inputs::setMouseWheel),
			Locator::CmdNode("GetMouseWheel", this, &Inputs::getMouseWheel),
			CNEND
			);

	Locator::Get().Add(tree);		

	Locator::Get().Add(Locator::CmdNode{ "Update", this, &Inputs::update });

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
		Locator::Get().Remove(tree);
		isIgnored = true;
	}

	if(!ignore && isIgnored)
	{
		Locator::Get().Add(tree);
		isIgnored = false;
	}
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

	mouseMove = dVec2(0.0); 
}

void Inputs::getKey(SDL_Scancode code, State &state)
{ 
	state = keyStates[code];
}

void Inputs::getMouseButton(byte button, State &state)
{ state = buttonStates[button]; }

void Inputs::getMousePos(dVec2 &v)
{ v = mousePos; }

void Inputs::getMouseMove(dVec2 &v)
{ v = mouseMove; }

void Inputs::getMouseWheel(dVec2 &v)
{ v = mouseWheelMove; }

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
{ Locator::Get().Add(tree); }

void Inputs::Logger::stop()
{ Locator::Get().Remove(tree); }

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

	Locator::Get().Add(recordTree); 

	actionStartTime = Time::ProgramTime();
	isRecording = true;
}

void Inputs::Recorder::stopRecording()
{ 
	if(isReplaying) { stopReplaying(); }

	Locator::Get().Remove(recordTree);
	isRecording = false;
}

void Inputs::Recorder::startReplaying()
{
	inputs.ignoreInputs(true);

	if(isReplaying) { stopReplaying(); }
	if(isRecording) { stopRecording(); }

	Locator::Get().Add(replayTree); 

	actionStartTime = Time::ProgramTime();
	replayCurrentInput = 0;
	isReplaying = true;
}

void Inputs::Recorder::stopReplaying()
{
	inputs.ignoreInputs(false);

	if(isRecording)
	{ stopRecording(); }

	Locator::Get().Remove(replayTree);
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
	{ event.key.keysym.sym = std::any_cast<SDL_Scancode>(nextInput.Value); } break;

	case SDL_MOUSEBUTTONUP:
	case SDL_MOUSEBUTTONDOWN:
	{ event.button.button = std::any_cast<Uint8>(nextInput.Value); } break;

	case SDL_MOUSEMOTION:
	{ 
		event.motion.x = std::any_cast<dVec2>(nextInput.Value).x;
		event.motion.y = std::any_cast<dVec2>(nextInput.Value).y;
	} break;

	case SDL_MOUSEWHEEL:
	{
		event.wheel.x = std::any_cast<dVec2>(nextInput.Value).x;
		event.wheel.y = std::any_cast<dVec2>(nextInput.Value).y;
	} break;
	}

	SDL_PushEvent(&event);
}