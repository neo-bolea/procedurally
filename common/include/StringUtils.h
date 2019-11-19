#pragma once

#include <iterator>
#include <sstream>
#include <string>

namespace Strings
{
	std::string GetExtension(const std::string &str);

	//See https://stackoverflow.com/questions/5288396/c-ostream-out-manipulation/5289170#5289170 for more information
	template <typename Range, typename Value = typename Range::value_type>
	std::string Join(Range const& elements, const char *const delimiter)
	{
		std::ostringstream os;
		auto b = begin(elements), e = end(elements);

		if (b != e)
		{
			std::copy(b, prev(e), std::ostream_iterator<Value>(os, delimiter));
			b = prev(e);
		}
		if (b != e) { os << *b; }

		return os.str();
	}

	//See above for more information
	template <typename Input, typename Output, typename Value = typename Output::value_type>
	void Split(char delimiter, Output &output, Input const& input)
	{
		for (auto cur = std::begin(input), beg = cur; ; ++cur)
		{
			if (cur == std::end(input) || *cur == delimiter || !*cur)
			{
				output.insert(output.end(), Value(beg, cur));
				if (cur == std::end(input) || !*cur) { break; }
				else { beg = std::next(cur); }
			}
		}
	}
}