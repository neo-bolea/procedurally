#include <ostream>
#include <vector>

template<typename Type, typename Alloc>
std::ostream &operator<<(std::ostream &os, const std::vector<Type, Alloc> &v)
{ 
	os << '['; 
	for(auto &it = v.begin(); it != v.end(); it++)
	{ 
		os << *it; 
		if (it != v.end() - 1) { os << ','; }
	} 
	os << ']'; 

	return os; 
}

template<typename Type, typename Alloc>
std::istream &operator>>(std::istream &is, std::vector<Type, Alloc> &v)
{ 
	std::stringstream ss;
	ss << is.rdbuf();    

	int tell;
	if(ss.peek() == '[')
	{
		tell = ss.tellg();
		ss.get();
		tell = ss.tellg();
		v.clear();
		do
		{
			Type d;
			ss >> d;
			tell = ss.tellg();
			v.push_back(d);
		} while(ss.get() != ']');
	}

	return is; 
}