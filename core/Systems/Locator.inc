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
	{ 
		const type_info &givenArgsInfo = typeid(std::tuple<std::decay_t<Args>...>);
		it->second.func(&tuple, givenArgsInfo.hash_code(), givenArgsInfo.name());
	}
}

template<size_t N>
void Locator::Remove(const char (&key)[N], LeafFunc func)
{
	//TODO: Implement
}