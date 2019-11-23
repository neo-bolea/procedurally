#pragma once

#include <chrono>

class Time
{
public:
	using ChronoPoint = std::chrono::time_point<std::chrono::steady_clock>;
	using TimePoint = double;

	Time();

	static double ProgramTime();
	static double DeltaTime();

private:
	static ChronoPoint startTime;
	static double programTime;
	static double deltaTime;

	void update();
};