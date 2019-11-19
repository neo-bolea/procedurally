template<size_t N, typename ...Args>
constexpr void Locator::Call(const char (&key)[N], Args &&...args)
{ 
	Call(cmdKeySequenceHelper::toID(&key[0], N), args...);
}

template<typename ...Args>
void Locator::Call(const cmdKeySequenceHelper::id id, Args &&...args)
{ 
	std::tuple<Args&...> tuple(args...);
	for(auto it = cmds.find(id); it != cmds.end(); it++) 
	{ it->second(&tuple, typeid(std::tuple<Args...>)); } 
}