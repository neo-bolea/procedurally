#pragma once

#include <chrono>
#include <string>

class Watch
{
public:
	using TimeType = double;
	enum Format { ns, us, ms, s };

	Watch(Format format = ns);

	void Start(), Stop();

	TimeType Time(), TimeTotal(), TimeAvg();

	std::string sTime(), sTimeTotal(), sTimeAvg();
	void ClearPrev();

private:
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

	TimeType cast(TimeType d);
	TimePoint getTime();
	std::string toStr(TimeType t);

	static const TimeType dur[4];
	static const std::string formatStr[4];

	Format format;
	TimePoint start;
	TimeType diff = 0; 
	TimeType total = 0; 
	int counter = 0;
};