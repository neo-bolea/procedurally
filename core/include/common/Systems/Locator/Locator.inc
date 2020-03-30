template<size_t N, typename ...Args>
constexpr void Locator::Call(const char (&key)[N], Args &&...args)
{ 
	Call(locatorHasher::toID(&key[0], N - 1), args...);
}

template<typename ...Args>
void Locator::Call(const locatorHasher::id id, Args &&...args)
{
	// Erase all functions that were noted down by Remove().
	// Those functions can only be removed when the map isn't being accessed
	// as otherwise errors will be thrown.
	if(callStackSize == 0 && !cmdsToRemove.empty())
	{
		cleanRemoves();
	}

	std::tuple<Args&...> tuple(args...);
	auto funcs = cmds.equal_range(id);
	for (auto it = funcs.first; it != funcs.second; it++)
	{ 
		callStackSize++;

		const type_info &givenArgsInfo = typeid(std::tuple<std::decay_t<Args>...>);
		it->second.func(&tuple, givenArgsInfo.hash_code(), givenArgsInfo.name());

		callStackSize--;
		assert(callStackSize >= 0);
	}
}