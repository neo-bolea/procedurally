#include "Common/GameTime.h"

#include "Common/Systems/Locator/Locator.h"

#include "SDL.h"

using namespace std::chrono;

Time::Time()
{
	Locator::Add(Locator::CmdNode("Update", this, &Time::update));
	startTime = steady_clock::now();
}

static Uint32 next_time;

bool Time::GetLimitFPS() { return LimitFPS; }
void Time::SetLimitFPS(bool limitFPS) 
{ 
	nextLimitedFrame = programTime;
	LimitFPS = limitFPS;
}

int Time::GetMaxFPS() { return MaxFPS; }
void Time::SetMaxFPS(int maxFPS) { MaxFPS = maxFPS; }

void Time::sleep()
{
	SDL_Delay((nextLimitedFrame <= programTime) ? 0 : (nextLimitedFrame - programTime));
	while (nextLimitedFrame <= programTime - (1.0 / MaxFPS))
	{ nextLimitedFrame += (1000.0 / MaxFPS); }
}

void Time::update()
{
	if(LimitFPS) { sleep(); }

	{
		nanoseconds diff = steady_clock::now() - startTime;
		programTime = diff.count() / 1'000'000'000.0;
	}

	{
		static ChronoPoint lastTime = startTime;
		deltaTime = (steady_clock::now() - lastTime).count() / 1'000'000'000.0;
		lastTime = steady_clock::now();
	}
}

double Time::ProgramTime()
{
	return programTime;
}

double Time::DeltaTime()
{
	return deltaTime;
}