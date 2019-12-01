#pragma once

#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

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
	void Split(char delimiter, Output &output, Input const& input, size_t start = 0, size_t end = 0)
	{
		if (end == 0) { end = std::size(input); }
		size_t last = (std::size(input) - end);
		for (auto cur = std::begin(input) + start, beg = cur; ; ++cur)
		{
			if (cur == std::end(input) - last || *cur == delimiter || !*cur)
			{
				output.insert(output.end(), Value(beg, cur));
				if (cur == std::end(input) || !*cur) { break; }
				else { beg = std::next(cur); }
			}
		}
	}

	template <typename Input>
	void Split1(
		char delimiter, 
		std::vector<std::string_view> &output, 
		Input &input, 
		size_t start1 = 0, size_t end1 = 0)
	{
		if (end1 == 0) { end1 = std::size(input); }
		size_t last = (std::size(input) - end1);
		Input::iterator start = input.begin() + start1;
		Input::iterator end = input.end();
		Input::iterator next = std::find(start, end, delimiter);
		while (next != std::end(input) - last) 
		{
			output.push_back(std::string_view(&(*start), next - start));
			start = next + 1;
			next = std::find(start, end, delimiter);
		}
		output.push_back(std::string_view(&(*start), next - start));
	}
}