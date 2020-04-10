#pragma once

#include <chrono>

class Time
{
public:
	using ChronoPoint = std::chrono::time_point<std::chrono::steady_clock>;
	using TimePoint = double;

	Time();

	// TODO: Stop time from continuing when application is stopped (e.g. while debugging).
	static double ProgramTime();
	static double DeltaTime();

	static bool GetLimitFPS();
	static void SetLimitFPS(bool limitFPS);

	static int GetMaxFPS();
	static void SetMaxFPS(int maxFPS);

private:
	inline static ChronoPoint startTime;
	inline static double programTime;
	inline static double deltaTime;

	inline static bool LimitFPS = false;
	inline static int MaxFPS = 144;
	inline static double nextLimitedFrame = 0;

	void sleep(), update();
};