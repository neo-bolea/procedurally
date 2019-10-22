#pragma once

//namespace CHECK { struct No {}; }
//
//#define ENABLE_IF_OPERATOR(Name, Operator) namespace CHECK									     \
//{																												     \
//	template<typename T, typename Arg> No operator Operator (const T&, const Arg&);		  \
//																												     \
//	template<typename T, typename Arg = T>															     \
//	struct Name																					              \
//	{																											     \
//		enum { value = !std::is_same<decltype(*(T*)(0) Operator *(Arg*)(0)), No>::value }; \
//	};  																										     \
//}		
//
//ENABLE_IF_OPERATOR(EqualExists, ==);
//ENABLE_IF_OPERATOR(UnequalExists, !=);
//ENABLE_IF_OPERATOR(AssignExists, =);
//
//ENABLE_IF_OPERATOR(AddExists, +);
//ENABLE_IF_OPERATOR(SubExists, -);
//ENABLE_IF_OPERATOR(MultExists, *);


// See https://rextester.com/ONP80107 for more information.
#define PP_CAT( A, B ) A ## B
#define PP_EXPAND(...) __VA_ARGS__

// Macro overloading feature support
#define PP_VA_ARG_SIZE(...) PP_EXPAND(PP_APPLY_ARG_N((PP_ZERO_ARGS_DETECT(__VA_ARGS__), PP_RSEQ_N)))

#define PP_ZERO_ARGS_DETECT(...) PP_EXPAND(PP_ZERO_ARGS_DETECT_PREFIX_ ## __VA_ARGS__ ## _ZERO_ARGS_DETECT_SUFFIX)
#define PP_ZERO_ARGS_DETECT_PREFIX__ZERO_ARGS_DETECT_SUFFIX ,,,,,,,,,,,0

#define PP_APPLY_ARG_N(ARGS) PP_EXPAND(PP_ARG_N ARGS)
#define PP_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N,...) N
#define PP_RSEQ_N 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define PP_OVERLOAD_SELECT(NAME, NUM) PP_CAT( NAME ## _, NUM)
#define PP_MACRO_OVERLOAD(NAME, ...) PP_OVERLOAD_SELECT(NAME, PP_VA_ARG_SIZE(__VA_ARGS__))(__VA_ARGS__)