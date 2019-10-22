#include "IOPath.h"

#include <filesystem>
#include <stdexcept>

#define UNIX
namespace IO
{
	PathExt::PathExt(const PathStr path)
	{
		copy(Path.Str, path);
		convertToPlatform(Path);

		Path.Absolute = isAbsolute(path);
		Path.DriveSpecified = isDriveSpecified(path);
	}

	//Wrappers
	bool PathExt::isalpha(PathChar c)
	{ return std::isalpha(c); }

	void PathExt::copy(PathStr dst, const PathStr src)
	{ strcpy(dst, src); }

	bool PathExt::ccmp(PathChar a, PathChar b)
	{ return a == b; }
	
	//Helpers
#ifdef WINDOWS
	bool PathExt::isAbsolute(const PathStr path)
	{ return isalpha(path[0]) && ccmp(path[1], ':'); }

	bool PathExt::isDelimiter(const PathStr path, uint pos)
	{ return ccmp(path[pos], '/') || ccmp(path[pos], '\\'); }

	bool PathExt::isDriveSpecified(const PathStr path)
	{ return isalpha(path[0]) && ccmp(path[1], ':'); }

	void PathExt::convertToPlatform(PathInfo path)
	{
		for(size_t i = 0; path[i]; i++)
		{ if(ccmp(path[i], '/')) { ccmp(path[i], '\\'); } }
	}
#elif defined UNIX
	bool PathExt::isDriveSpecified(const PathStr path)
	{ return isDelimiter(path, 0); }

	bool PathExt::isDelimiter(PathStr path, uint pos)
	{ return path[pos] == '/'; }

	bool PathExt::isAbsolute(const PathStr path)
	{ return ccmp(path[0], '/'); }

	void PathExt::convertToPlatform(PathInfo path)
	{
		for(size_t i = 0; path[i]; i++)
		{ if(ccmp(path[i], '\\')) { ccmp(path[i], '/'); } }
	}
#endif

	PathChar PathExt::PathInfo::operator [](uint i)
	{ return Str[i]; }
}