template<size_t N, typename ...Args>
constexpr void Locator::Call(const char (&key)[N], Args &&...args)
{ 
	Call(locatorHasher::toID(&key[0], N - 1), args...);
}

template<typename ...Args>
void Locator::Call(const locatorHasher::id id, Args &&...args)
{ 
	std::tuple<Args&...> tuple(args...);
	auto funcs = cmds.equal_range(id);
	for (auto it = funcs.first; it != funcs.second; it++)
	{ it->second(&tuple, typeid(std::tuple<Args...>)); } 
}