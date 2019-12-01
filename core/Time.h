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

	static bool GetLimitFPS();
	static void SetLimitFPS(bool limitFPS);

	static int GetMaxFPS();
	static void SetMaxFPS(int maxFPS);

private:
	static ChronoPoint startTime;
	static double programTime;
	static double deltaTime;

	static bool LimitFPS;
	static int MaxFPS;
	static double nextLimitedFrame;

	void sleep(), update();
};