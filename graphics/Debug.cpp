#include "Debug.h"

#include <iostream>

namespace Debug
{
	void Log(const std::string& str, InfoType type) { std::cout << str << std::endl; }
	void Log(const std::string &str, InfoType type, std::initializer_list<const char *> &&tags) { std::cout << str << std::endl; }
	void Log(const std::string &str, InfoType type, std::vector<const char *> &&tags) { std::cout << str << std::endl; }
}