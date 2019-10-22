#include <cctype>
#include <memory>
#include <vector>

#define WINDOWS

namespace IO
{
	using PathChar = char;
	using PathStr = PathChar *;

	class PathExt
	{
	public:
		struct PathInfo
		{
			PathChar operator [](uint i);

			PathStr Str;
			bool Absolute, DriveSpecified;
		};

		PathExt(const PathStr path);

		PathInfo Path;

	private:
		//Wrappers
		bool isalpha(PathChar c);
		void copy(PathStr dst, const PathStr src);
		bool ccmp(PathChar a, PathChar b);

		//Helper Functions
		bool isAbsolute(const PathStr path);
		bool isDelimiter(const PathStr path, uint pos);
		bool isDriveSpecified(const PathStr path);
		void convertToPlatform(PathInfo path);
	};
}