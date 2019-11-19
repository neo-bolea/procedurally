//See https://www.daniweb.com/programming/software-development/code/482276/c-11-compile-time-string-concatenation-with-constexpr for more information.
//NOTE: This code has been modified and does not represent the origial source code.

#pragma once

#include <assert.h>
#include <cstddef>
#include <cstring>

class StaticStr 
{
private:
	const char *const text_ptr;
	unsigned int text_size;
	const StaticStr *const head;

	constexpr char GetCharFromHead(unsigned int i, unsigned int hd_size) const 
	{
		return (i < hd_size ? 
			(*head)[i] 
			: (i < (hd_size + text_size) ? text_ptr[i - hd_size] : '\0'));
	};

	static constexpr std::size_t fnv_prime  
		= (sizeof(size_t) == 8 ? 1099511628211u : 16777619u);
	static constexpr std::size_t fnv_offset 
		= (sizeof(size_t) == 8 ? 14695981039346656037u : 2166136261u);

	constexpr std::size_t FNV1aHash(unsigned int i) const 
	{
		return (i == 0 ? 
			(head != nullptr ? 
				  ((head->FNV1aHash(head->text_size-1) ^ text_ptr[0]) * fnv_prime) :
			 fnv_offset) :
			 ((FNV1aHash(i-1) ^ text_ptr[i]) * fnv_prime));
	};

	template <typename FwdIter>
	void CopyToRecurse(FwdIter &beg, FwdIter end) const
	{
		if(head != nullptr) { head->CopyToRecurse(beg, end); }
		for(unsigned int i = 0; (i < text_size) && (beg != end); ++i, ++beg)
		{ *beg = text_ptr[i]; }
	};

	void CopyToRecurse(char *&beg, char *end) const
	{
		if(head != nullptr) { head->CopyToRecurse(beg, end); }
		std::size_t sz_to_cpy = (end - beg < text_size ? end - beg : text_size);
		std::memcpy(beg, text_ptr, sz_to_cpy);
		beg += sz_to_cpy;
	};

	constexpr StaticStr(const char* aStr, unsigned int N,
										const StaticStr* aHead = nullptr) 
		: text_ptr(aStr), text_size(N), head(aHead) { };

public:
	template <unsigned int N>
	constexpr StaticStr(const char(&aStr)[N],
										const StaticStr* aHead = nullptr) : 
		text_ptr(aStr), text_size(N-1), head(aHead)
	{ static_assert(N >= 1, "Invalid string literal! Length is zero!"); };

	constexpr unsigned int size() const 
	{ return text_size + (head != nullptr ? head->size() : 0); };

	constexpr char operator[](unsigned int i) const
	{
		assert(i < text_size, "Indexed character is over the string's character limit!");
		return (head != nullptr ? 
				  GetCharFromHead(i, head->size()) : 
				  text_ptr[i]);
	};

	template <unsigned int N>
	constexpr StaticStr operator+(const char(&aHead)[N]) const 
	{ return StaticStr(aHead, this); };

	constexpr StaticStr operator+(const StaticStr &aHead) const 
	{ return StaticStr(aHead.text_ptr, aHead.text_size, this); };

	constexpr std::size_t Hash() const 
	{ return FNV1aHash(text_size - 1); };

	template <typename FwdIter>
	void CopyTo(FwdIter beg, FwdIter end) const 
	{ CopyToRecurse(beg, end); };

	void CopyTo(char* beg, char *end) const
	{ CopyToRecurse(beg, end); };
};