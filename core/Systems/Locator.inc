#pragma once

template<size_t SIZE, typename ...Args>
constexpr void Locator::Call(const CmdKey (&keys)[SIZE], Args &&...args)
{ Call(keySequences::toID<SIZE, 0>(keys), args...); }

template<typename ...Args>
void Locator::Call(const keySequences::id id, Args &&...args)
{ 
	std::tuple<Args&...> tuple(args...);
	for(size_t i = 0; i < cmds[id].size(); i++) 
	{ cmds[id][i](&tuple, typeid(std::tuple<Args...>)); } 
}