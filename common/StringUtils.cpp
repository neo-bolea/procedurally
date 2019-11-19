#include "StringUtils.h"

namespace Strings
{
	std::string GetExtension(const std::string &str)
	{
		for(int64 i = str.size() - 1; i >= 0; i--)
		{
			if(str[i] == '.') { return str.substr(i); }
			if(str[i] == '/' || str[i] == '\\') { return ""; }
		}
	
		return "";
	}
}