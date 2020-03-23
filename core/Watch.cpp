#include "Common/Watch.h"

Watch::Watch(Format format) : format(format) {}
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

Watch::TimeType Watch::Time() { return cast(diff); }
Watch::TimeType Watch::TimeTotal() { return cast(total); }
Watch::TimeType Watch::TimeAvg() { return cast(counter ? (total / counter) : 0); }

std::string Watch::sTime()      { return toStr(Time()); }
std::string Watch::sTimeTotal() { return toStr(TimeTotal()); }
std::string Watch::sTimeAvg()   { return toStr(TimeAvg()); }
void Watch::ClearPrev() { total = 0; counter = 0; }

Watch::TimeType Watch::cast(TimeType d)
{ return d / dur[format]; }

std::string Watch::toStr(TimeType t) 
{ return std::to_string(t) + formatStr[format]; }

Watch::TimePoint Watch::getTime()
{ return std::chrono::high_resolution_clock::now(); }

const Watch::TimeType Watch::dur[4] = { 1e+0, 1e+3, 1e+6, 1e+9 };
const std::string Watch::formatStr[4] = { "ns", "µs", "ms", "s" };