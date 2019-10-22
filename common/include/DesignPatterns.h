#pragma once

template<typename T>
class Singleton
{
public:
	static T &Get()
	{
		static T inst;
		return inst;
	}

	Singleton<T>(const Singleton<T> &) = delete;
	void operator =(const Singleton<T> &) = delete;

protected:
	Singleton<T>() {}
};