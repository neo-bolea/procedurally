#include "Common/Watch.h"

//TODO: Make it so the watch doesnt have to be stopped for it to be read from.

Watch::Watch(Format format) : Format_(format) {}
void Watch::Start() 
{ 
	start = getTime(); 

	started = true; 
}

void Watch::Stop()
{
	if(!started) 
	{
		diff = static_cast<TimeType>(0);
		return;
	}
	diff = (TimeType)(getTime() - start).count(); 
	total += diff;
	counter++;

	started = false; 
}

template<typename F, typename ...Args>
Watch::TimeType Watch::Measure(F &&func, Args &&...args)
{
	Start();
	std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
	Stop();
	return Time();
}

Watch::TimeType Watch::Time() { return cast(diff); }
Watch::TimeType Watch::TimeTotal() { return cast(total); }
Watch::TimeType Watch::TimeAvg() { return cast(counter ? (total / counter) : 0); }

std::string Watch::sTime()      { return toString(Time()); }
std::string Watch::sTimeTotal() { return toString(TimeTotal()); }
std::string Watch::sTimeAvg()   { return toString(TimeAvg()); }
void Watch::ClearPrev() { total = 0; counter = 0; }

Watch::TimeType Watch::cast(TimeType d)
{ return d / dur[Format_]; }

std::string Watch::toString(TimeType t) 
{ return std::to_string(t) + formatStr[Format_]; }

Watch::TimePoint Watch::getTime()
{ return std::chrono::high_resolution_clock::now(); }

const Watch::TimeType Watch::dur[4] = { 1e+0, 1e+3, 1e+6, 1e+9 };
const std::string Watch::formatStr[4] = { "ns", "µs", "ms", "s" };