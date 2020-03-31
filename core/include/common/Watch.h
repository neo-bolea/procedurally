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

	template<typename F, typename ...Args>
	TimeType Measure(F &&func, Args &&...args);

	TimeType Time(), TimeTotal(), TimeAvg();
	std::string sTime(), sTimeTotal(), sTimeAvg();
	void ClearPrev();

	Format Format_;
private:
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

	TimeType cast(TimeType d);
	TimePoint getTime();
	std::string toString(TimeType t);

	static const TimeType dur[4];
	static const std::string formatStr[4];

	TimePoint start;
	bool started = false;
	TimeType diff = 0; 
	TimeType total = 0; 
	int counter = 0;
};