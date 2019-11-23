template<typename ...Args>
Locator::CmdNode::CmdNode(const char *cmd, const Args &...inners) : levelKey(cmd)
{
	std::vector<CmdNode> innersV = { inners... };
	for(size_t i = 0; i < innersV.size(); i++)
	{ nodes.push_back(innersV[i]); }
}

template<typename Signature, typename ...FuncArgs>
void Locator::CmdNode::variableFunctionSignature(
	void *args, 
	size_t givenArgsInfo, const char *givenArgsName, 
	size_t expectedArgsInfo, const char *expectedArgsName, 
	Signature func)
{ 
	//TODO: Check for correct arguments.
	//const type_info &correctInfo1 = typeid(std::tuple<SDL_Event&>);
	//const type_info &correctInfo = typeid(std::tuple<FuncArgs&...>);
	//assert(info == correctInfo1);
	std::tuple<FuncArgs&...> tupledArgs = *static_cast<std::tuple<FuncArgs&...> *>(args);
	//const type_info &t1 = typeid(tupledArgs);
	////const type_info &t2 = typeid(&func.target_type());
	//std::cout << t1.name << " vs " << std::endl;

	//const type_info &argInfo = typeid(std::decay_t<FuncArgs>...);
	//std::cout << argInfo.name() << " vs " << givenArgsInfo.name() << std::endl;
	if(givenArgsInfo == expectedArgsInfo)
	{
		std::apply(func, tupledArgs);
	}
	else
	{
		std::cout << "Wrong arguments given for the called function: " << "\n"
			<< "Expected " << expectedArgsName << " but got " << givenArgsName << std::endl;
	}
}

template<typename ...FuncArgs>
Locator::CmdNode::CmdNode(const char *cmd, void(*cb)(FuncArgs...)) 
	: levelKey(cmd) 
{
	using Signature = std::function<void(FuncArgs...)>;

	const type_info &expectedArgsInfo = typeid(std::tuple<std::decay_t<FuncArgs>...>);
	leafFunc = std::bind(variableFunctionSignature<Signature, std::decay_t<FuncArgs...>>, 
								std::placeholders::_1, 
								std::placeholders::_2, 
								std::placeholders::_3, 
								expectedArgsInfo.hash_code(),
								expectedArgsInfo.name(),
								cb);
}

template<typename ToBind, typename ...FuncArgs>
Locator::CmdNode::CmdNode(const char *cmd, ToBind *toBind, void(ToBind::*cb)(FuncArgs...)) 
	: levelKey(cmd) 
{
	using Signature = std::function<void(FuncArgs...)>;
	Signature boundFunc = BindFirst(cb, toBind);

	const type_info &expectedArgsInfo = typeid(std::tuple<std::decay_t<FuncArgs>...>);
	LeafFunc::FuncType
		func = std::bind(variableFunctionSignature<Signature, FuncArgs...>, 
							  std::placeholders::_1, 
							  std::placeholders::_2, 
							  std::placeholders::_3, 
							  expectedArgsInfo.hash_code(),
							  expectedArgsInfo.name(),
							  boundFunc);
	leafFunc = LeafFunc(func, toBind);
}