#include "Debug.h"

namespace Debug
{
	void Log(const std::string &str, InfoType type) {}
	void Log(const std::string &str, InfoType type, std::initializer_list<const char *> &&tags) {}
	void Log(const std::string &str, InfoType type, std::vector<const char *> &&tags) {}
}