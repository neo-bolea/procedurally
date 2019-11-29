#include "Time.h"

#include "Systems/Locator.h"

using namespace std::chrono;

Time::ChronoPoint Time::startTime;
double Time::programTime;
double Time::deltaTime;

Time::Time()
{
	Locator::Add(Locator::CmdNode("Update", this, &Time::update));

	startTime = std::chrono::steady_clock::now();
}

void Time::update()
{
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