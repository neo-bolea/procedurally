#pragma once

#include "Debug.h"

#include <vector>
#include <string>
#include <unordered_map>

template<typename Derived, typename T>
class Pool
{
public:
	typedef std::shared_ptr<T> TRef;

	///<summary> Create a program given paths to the shaders. </summary>
	///<returns> Returns the created program. If a program with identical paths exists, it is returned instead. </returns>
	template<typename ...Args>
	TRef Load(Args... args)
	{
		size_t hash = ((Derived *)this)->GetHash(args...);

		auto iter = pathRefs.find(hash);
		if(iter != pathRefs.end())
		{ return iter->second; }	//The program already exists, return it.
		else
		{	//The program doesn't exist yet, create a new one.
			//Push it back into the list of references.
			std::shared_ptr<T> ref = Create(args...);
			pathRefs[hash] = ref;	

			return ref;
		}
	}

	///<summary> Create a program with a name, so it can be referenced by name. </summary>
	///<param name="name"> The name to assign to the program. 
	///	If a name with that name already exists, an error is raised. </param>
	template<typename ...Args>
	TRef LoadByName(const std::string &name, Args... args)
	{
		auto iter = nameRefs.find(name);
		if(iter != nameRefs.end())
		{ 
			//Debug::Log("An object with that name already exists!", Debug::Error, ((Derived *)this)->tags);
			return iter->second;
		}

		//Create both a path and name reference.
		std::shared_ptr<T> ref = Load(args...);
		nameRefs[name] = ref;

		return ref;
	}

	///<summary> Create a program with no name. The reference to the texture must be kept to be referenced again.</summary>
	template<typename ...Args>
	TRef Create(Args... args)
	{
		//Create a name reference.
		T t(args..., (Derived *)this);
		pool.push_back(t);	

		return std::make_shared<T>(pool.back());
	}

	///<summary> Create a program with a name, so it can be referenced by name. </summary>
	///<param name="name"> The name to assign to the program. 
	///	If a name with that name already exists, an error is raised. </param>
	template<typename ...Args>
	TRef CreateByName(const std::string &name, Args... args)
	{
		auto iter = nameRefs.find(name);
		if(iter != nameRefs.end())
		{ 
			//Debug::Log("An object with that name already exists!", Debug::Error, ((Derived *)this)->tags);
			return iter->second;
		}

		//Push it back into the list of names.
		std::shared_ptr<T> ref = Create(args...);
		nameRefs[name] = ref;	

		return ref;
	}

	TRef Get(const std::string &name)
	{
		auto iter = nameRefs.find(name);
		if(iter == nameRefs.end())
		{ 
			//Debug::Log("An object with that name doesn't exist!", Debug::Error, ((Derived *)this)->tags);
			return nullptr;
		}

		return iter->second;
	}

private:
	std::unordered_map<size_t, TRef> pathRefs;
	std::unordered_map<std::string, TRef> nameRefs;
	std::vector<T> pool;
};