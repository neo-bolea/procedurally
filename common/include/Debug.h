#pragma once

#include <initializer_list>
#include <string>
#include <unordered_set>
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

	void Log(const std::string &str, InfoType type = Debug::Info);
	void Log(const std::string &str, InfoType type, std::initializer_list<const char *> &&tags);
	void Log(const std::string &str, InfoType type, std::vector<const char *> &&tags);
}

class UniqueMessageRaiser
{
protected:
	using StringHash = uint;

	void log(const std::string &str, Debug::InfoType type = Debug::Info) const;
	void log(const std::string &str, Debug::InfoType type, std::initializer_list<const char *> &&tags) const;

private:
	StringHash Hash(const std::string &str) const;

	mutable std::unordered_set<StringHash> raisedMessages;
};