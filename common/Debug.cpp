#include "Debug.h"

#include "Hash.h"

#include <iostream>

namespace Debug
{
	void Log(const std::string &str, InfoType type) { std::cout << str << std::endl; }
	void Log(const std::string &str, InfoType type, std::initializer_list<const char *> &&tags) { std::cout << str << std::endl; }
	void Log(const std::string &str, InfoType type, std::vector<const char *> &&tags) { std::cout << str << std::endl; }
}

void UniqueMessageRaiser::log(const std::string &str, Debug::InfoType type) const
{
	StringHash hash = Hash(str);
	if(raisedMessages.count(hash) != 0) { return; }

	raisedMessages.insert(hash);
	Debug::Log(str, type);
}

void UniqueMessageRaiser::log(const std::string &str, Debug::InfoType type, std::initializer_list<const char *> &&tags) const
{
	StringHash hash = Hash(str);
	if(raisedMessages.count(hash) != 0) { return; }

	raisedMessages.insert(hash);
	Debug::Log(str, type, tags);
}

UniqueMessageRaiser::StringHash UniqueMessageRaiser::Hash(const std::string &str) const
{
	return CRC32::Get(str.data(), str.size());
}