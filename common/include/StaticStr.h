////See https://www.daniweb.com/programming/software-development/code/482276/c-11-compile-time-string-concatenation-with-constexpr for more information.
////NOTE: This code has been modified and does not represent the origial source code.
//
//#pragma once
//
//#include <assert.h>
//#include <cstddef>
//#include <cstring>
//
//class StaticStr 
//{
//private:
//	const char *textPtr;
//	size_t textSize;
//	const StaticStr *head;
//
//	constexpr char getCharFromHead(size_t i) const 
//	{
//		if(i < head->size())
//		{ return (*head)[i]; }
//		else 
//		{
//			if(i < (head->size() + textSize))
//			{ return textPtr[i - head->size()]; }
//			else 
//			{ return '\0'; }
//		}
//	};
//
//	static constexpr size_t fnv_prime  
//		= (sizeof(size_t) == 8 ? 1099511628211u : 16777619u);
//	static constexpr size_t fnv_offset 
//		= (sizeof(size_t) == 8 ? 14695981039346656037u : 2166136261u);
//
//	constexpr size_t fnv1aHash(size_t i) const 
//	{
//		return (i == 0 ? 
//			(head != nullptr ? 
//				  ((head->fnv1aHash(head->textSize-1) ^ textPtr[0]) * fnv_prime) :
//			 fnv_offset) :
//			 ((fnv1aHash(i-1) ^ textPtr[i]) * fnv_prime));
//	};
//
//	template <typename FwdIter>
//	void copyToRecurse(FwdIter &beg, FwdIter end) const
//	{
//		if(head != nullptr) { head->copyToRecurse(beg, end); }
//		for(size_t i = 0; (i < textSize) && (beg != end); ++i, ++beg)
//		{ *beg = textPtr[i]; }
//	};
//
//	void copyToRecurse(char *&beg, char *end) const
//	{
//		if(head != nullptr) { head->copyToRecurse(beg, end); }
//		size_t sz_to_cpy = (end - beg < textSize ? (end - beg) : textSize);
//		memcpy(beg, textPtr, sz_to_cpy);
//		beg += sz_to_cpy;
//	};
//
//	constexpr StaticStr(const char *aStr, size_t N, const StaticStr *aHead = nullptr) 
//		: textPtr(aStr), textSize(N), head(aHead) { };
//
//public:
//	template <size_t N>
//	constexpr StaticStr(const char(&aStr)[N], const StaticStr *aHead = nullptr) 
//		: textPtr(aStr), textSize(N-1), head(aHead)
//	{ static_assert(N >= 1, "Invalid string literal! Length is zero!"); };
//
//	constexpr StaticStr &operator=(const StaticStr &aHead)
//	{ 
//		textPtr = aHead.textPtr;
//		textSize = aHead.textSize;
//		head = aHead.head;
//
//		return *this;
//	};
//
//	constexpr char operator[](size_t i) const
//	{
//		assert(i < textSize, "Indexed character is over the string's character limit!");
//		return (head != nullptr ? getCharFromHead(i) : textPtr[i]);
//	};
//
//	template <size_t N>
//	constexpr StaticStr operator+(const char(&aHead)[N]) const 
//	{ return StaticStr(aHead, this); };
//
//	constexpr StaticStr operator+(const StaticStr &aHead) const 
//	{ return StaticStr(aHead.textPtr, aHead.textSize, this); };
//
//	constexpr size_t Hash() const 
//	{ return fnv1aHash(textSize - 1); };
//
//	constexpr size_t size() const 
//	{ return textSize + (head != nullptr ? head->size() : 0); };
//
//	template <typename FwdIter>
//	void CopyTo(FwdIter start, FwdIter end) const 
//	{ copyToRecurse(start, end); };
//
//	void CopyTo(char *start, char *end) const
//	{ copyToRecurse(start, end); };
//
//	//constexpr StaticStr TrimStart(const size_t newStart) const 
//	//{
//	//	//6     6
//	//	//Hello World!
//	//	//      World!
//	//	//
//	//	if(head == nullptr)
//	//	{
//	//		/*assert*/if(newStart < textSize)
//	//		{
//	//			return StaticStr(textPtr + newStart, textSize - newStart);
//	//		}
//	//	}
//	//	else
//	//	{
//	//		if(newStart < head->size())
//	//		{
//	//			return StaticStr(textPtr, textSize,);
//	//		}
//	//	}
//	//
//	//	//if(head != nullptr && newStart < head->size())
//	//	//{
//	//	//	return StaticStr(textPtr + newStart, (head->size() - newStart), head);
//	//	//}
//	//	//else 
//	//	//{
//	//	//	assert(newStart < ((head != nullptr ? head->size() : 0) + textSize), "Cannot trim string to after its end!");
//	//	//	return StaticStr(textPtr + newStart, textSize - newStart - 1);
//	//	//}
//	//}
//
//	//constexpr StaticStr TrimEnd(const size_t newEnd) const 
//	//{
//	//	if(head != nullptr && newEnd < head->size())
//	//	{
//	//		return StaticStr(textPtr + newStart, (head->size() - newStart), head);
//	//	}
//	//	else 
//	//	{
//	//		assert(newStart < (head->size() + textSize), "Cannot trim string to after its end!");
//	//		return StaticStr(head->textPtr + (newStart - head->size()), textSize - newStart - 1);
//	//	}
//	//}
//};
//
////constexpr StaticStr hello = "Hello ";
////constexpr StaticStr world = "World!";
////constexpr StaticStr lit_str = hello + "World!";
////constexpr StaticStr lit_str2 = hello + world;
////constexpr StaticStr lit_str3 = lit_str.TrimStart(0);
////constexpr char c1[] = {
////	lit_str3[0],
////	lit_str3[1],
////	lit_str3[2],
////	lit_str3[3],
////	lit_str3[4],
////	lit_str3[5],
////	lit_str3[6]
////};
////constexpr size_t h1 = lit_str.Hash();
//
////class LiteralString
////{
////public:
////	const size_t Size;
////	const LiteralString *first;
////	const char c;
////
////	template<size_t N>
////	constexpr LiteralString(const char(&str)[N]) : Size(N), c(str[0]), first(N > 0 ? LiteralString(str, N+1) : nullptr)
////	{
////		//first = LiteralString(str+1);
////	}
////
////	constexpr LiteralString(const char *str, size_t N) : Size(N), c(str[0]), first(N > 0 ? LiteralString(str, N+1) : nullptr)
////	{
////		//first = LiteralString(str+1);
////	}
////};
////
////constexpr LiteralString hello = "Hello ";
////constexpr LiteralString world = "World!";
////constexpr LiteralString lit_str = hello + "World!";
////constexpr LiteralString lit_str2 = hello + world;
////constexpr LiteralString lit_str3 = lit_str.TrimStart(0);