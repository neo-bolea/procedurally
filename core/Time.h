#pragma once

#include <chrono>

class Time
{
public:
	using ChronoPoint = std::chrono::time_point<std::chrono::steady_clock>;
	using TimePoint = double;

	static ChronoPoint StartTime;
	static double ProgramTime;
	static double DeltaTime;
};