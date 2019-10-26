template<typename ...Args>
Locator::CmdNode::CmdNode(CmdKey cmd, const Args &...inners) : levelKey(cmd)
{
	std::vector<CmdNode> innersV = { inners... };
	for(size_t i = 0; i < innersV.size(); i++)
	{ nodes.push_back(innersV[i]); }
}

template<typename Signature, typename ...FuncArgs>
void Locator::CmdNode::variableFunctionSignature(void *args, const type_info &info, Signature func) 
{ 
	//TODO: Check for correct arguments.
	//const type_info &correctInfo1 = typeid(std::tuple<SDL_Event&>);
	//const type_info &correctInfo = typeid(std::tuple<FuncArgs&...>);
	//assert(info == correctInfo1);
	std::tuple<FuncArgs&...> tupledArgs = *static_cast<std::tuple<FuncArgs&...> *>(args);
	std::apply(func, tupledArgs);
}

template<typename ...FuncArgs>
Locator::CmdNode::CmdNode(CmdKey cmd, void(*cb)(FuncArgs...)) 
	: levelKey(cmd) 
{
	using Signature = std::function<void(FuncArgs...)>;

	leafFunc = std::bind(variableFunctionSignature<Signature, FuncArgs...>, 
								std::placeholders::_1, 
								std::placeholders::_2, 
								cb);
}

template<typename Member, typename ...FuncArgs>
Locator::CmdNode::CmdNode(CmdKey cmd, Member *toBind, void(Member::*cb)(FuncArgs...)) : levelKey(cmd) 
{
	using Signature = std::function<void(FuncArgs...)>;

	Signature boundFunc = std::bind(cb, toBind, std::placeholders::_1);
	leafFunc = std::bind(variableFunctionSignature<Signature, FuncArgs...>, 
								std::placeholders::_1, 
								std::placeholders::_2, 
								boundFunc);
}