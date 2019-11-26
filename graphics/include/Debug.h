#pragma once

#include <initializer_list>
#include <string>
#include <vector>

#define UNDEFINED_CODE_PATH assert(false);

namespace Debug
{
	enum InfoType
	{
		Info,
		Warning,
		Error
	};

	void Log(const std::string &str, InfoType type);
	void Log(const std::string &str, InfoType type, std::initializer_list<const char *> &&tags);
	void Log(const std::string &str, InfoType type, std::vector<const char *> &&tags);
}