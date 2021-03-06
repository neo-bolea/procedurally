#pragma once

#include "Common/Debug.h"
#include "Math/MathExt.h"
#include "Common/Templates.h"
#include "Common/Vec.h"

#include <algorithm>
#include <assert.h>
#include <functional>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>

using IntVector2 = iVec2;

#define GRID_TEMPLATE_SIGNATURE template<typename T, size_t N, BorderPolicy Policy>
#define GRID_SGN Grid<T, N, Policy>

//TODO: Allow [] operator to work with vectors.
//TODO: Add Fill function (and possibly other utility functions).
//TODO: Make Proxy more readably by using decltype(auto) (remember how it returned all arguments as references...).

enum class BorderPolicy
{
	Undefined,
	Clamp,
	Value
};

template<typename T, BorderPolicy Policy, typename Enable = void>
class GridBase;

template<typename T, BorderPolicy Policy>
class GridBase<T, Policy, std::enable_if_t<Policy == BorderPolicy::Undefined>>
{};

template<typename T, BorderPolicy Policy>
class GridBase<T, Policy, std::enable_if_t<Policy == BorderPolicy::Clamp>>
{};

template<typename T, BorderPolicy Policy>
class GridBase<T, Policy, std::enable_if_t<Policy == BorderPolicy::Value>>
{ T BorderValue; };

template<typename T, size_t N = 2, BorderPolicy Policy = BorderPolicy::Clamp>
class Grid : public GridBase<T, Policy>
{
public:
	using CoordType = size_t;
	using Coords = Vec<CoordType, N>;
	using Dimensions = Coords;

	template<size_t CurN>
	struct Proxy;

	//// Constructors ////
	GRID_SGN();

	template<typename ...Args>
	GRID_SGN(Args ...dimensions);
	GRID_SGN(Dimensions dimensions);

	GRID_SGN(const GRID_SGN &arr);

	//// Indexing ////
	template<typename ...Args>
	const T &operator()(Args ...coords) const;
	
	template<typename ...Args>
	T &operator()(Args ...coords);
	
	const T &operator()(const Coords &coords) const;
	T &operator()(const Coords &coords);

	const Proxy<1> operator[](CoordType coord) const;
	Proxy<1> operator[](CoordType coord);

	void operator =(const GRID_SGN &other);
	void operator =(GRID_SGN &&other);

	//// Functions ////
	T EvaluateBorder(const Coords &coords);

	void ForAll(function_view<void(Coords)> func);

	//// Data ////
	std::vector<T> v;
	Dimensions Size;

private:
	template<size_t Depth>
	void forAll(function_view<void(Coords)> &func, Coords &coords);
};

GRID_TEMPLATE_SIGNATURE
template<size_t CurN>
struct GRID_SGN::Proxy
{
	GRID_SGN *grid;
	CoordType index;
	CoordType curMult;

	struct RefWrap
	{
		T &ref;
		void operator =(const T &value) { ref = value; }
		operator T &() { return ref; }
	};

	auto operator[](size_t coord)
	{
		curMult /= grid->Size[CurN];
		if constexpr (CurN + 1 == N)
		{ return RefWrap{ grid->v[index + coord * curMult] }; }
		else
		{
			assert(0 <= coord && coord <= grid->Size[CurN]);
			return Proxy<CurN + 1>{ grid, index + coord * curMult, curMult };
		}
	}
};


//// Constructors ////
GRID_TEMPLATE_SIGNATURE
GRID_SGN::Grid() : Size{} {}

GRID_TEMPLATE_SIGNATURE
template<typename ...Args>
GRID_SGN::Grid(Args ...dimensions) : Size{ static_cast<CoordType>(dimensions)... }
{ v.resize(std::accumulate(Size.e.begin(), Size.e.end(), 1, std::multiplies<double>())); }

GRID_TEMPLATE_SIGNATURE
GRID_SGN::Grid(Dimensions dimensions) : Size{ dimensions }
{ v.resize(std::accumulate(Size.e.begin(), Size.e.end(), 1, std::multiplies<double>())); }

GRID_TEMPLATE_SIGNATURE
GRID_SGN::Grid(const GRID_SGN &arr) : Size{ arr.Size }
{ this->v = arr.v; }


//// Indexing ////
GRID_TEMPLATE_SIGNATURE
template<typename ...Args>
const T &GRID_SGN::operator()(Args ...coords) const
{ return this->operator()(coords); }

GRID_TEMPLATE_SIGNATURE
template<typename ...Args>
T &GRID_SGN::operator()(Args ...coords)
{
	static_assert(!(sizeof...(Args) < N),
		"Grid cannot be accessed with less coordinates than dimensions.");
	static_assert(!(sizeof...(Args) > N),
		"Grid cannot be accessed with more coordinates than dimensions.");
	return this->operator()({ static_cast<CoordType>(coords)... }); 
}

GRID_TEMPLATE_SIGNATURE
const T &GRID_SGN::operator()(const Coords &coords) const
{ return this->operator()(coords); }

GRID_TEMPLATE_SIGNATURE
T &GRID_SGN::operator()(const Coords &coords)
{
	CoordType index = 0;
	CoordType currMult = v.size();
	for (size_t i = 0; i < N; i++)
	{
		currMult /= Size[i];
		if(0 <= coords[i] && coords[i] < Size[i])
		{ index += coords[i] * currMult; }
		else
		{ 
			T value = EvaluateBorder(coords);
			return value;
		}
	}
	return v[index];
}

GRID_TEMPLATE_SIGNATURE
const GRID_SGN::Proxy<1> GRID_SGN::operator[](CoordType coord) const
{ return this->operator[](coord); }

GRID_TEMPLATE_SIGNATURE
GRID_SGN::Proxy<1> GRID_SGN::operator[](CoordType coord)
{
	assert(0 <= coord && coord <= (Size[0] - 1));
	CoordType curMult = v.size() / Size[0];
	return { this, coord * curMult, curMult };
}

GRID_TEMPLATE_SIGNATURE
void GRID_SGN::operator =(const GRID_SGN &other)
{
	v = other.v;
	Size = other.Size;
}

GRID_TEMPLATE_SIGNATURE
void GRID_SGN::operator =(GRID_SGN &&other)
{
	v = std::move(other.v);
	Size = std::move(other.Size);
}


//// Functions ////
GRID_TEMPLATE_SIGNATURE
T GRID_SGN::EvaluateBorder(const Coords &coords)
{
	if constexpr (Policy == BorderPolicy::Undefined)
	{
		Debug::Log("Grid->operator(): Indices are out of range.", Debug::Error);
		return T();
	}
	else if constexpr (Policy == BorderPolicy::Clamp)
	{
		Coords clampedCoords;
		for (size_t i = 0; i < N; i++)
		{ clampedCoords[i] = Math::Clamp(coords[i], CoordType(0), Size[i] - 1); }
		return this->operator()(clampedCoords);
	}
	else if constexpr (Policy == BorderPolicy::Value)
	{ return BorderValue; }
	else
	{ UNDEFINED_CODE_PATH; }
}


GRID_TEMPLATE_SIGNATURE
void GRID_SGN::ForAll(function_view<void(Coords)> func)
{
	Coords coords{};
	forAll<0>(func, coords);
}

GRID_TEMPLATE_SIGNATURE
template<size_t Depth>
void GRID_SGN::forAll(function_view<void(Coords)> &func, Coords &coords)
{
	if constexpr (Depth == N)
	{ func(coords); }
	else
	{
		for (size_t i = 0; i < Size[Depth]; i++, coords[Depth]++)
		{ forAll<Depth + 1>(func, coords); }
		coords[Depth] = 0;
	}
}

//namespace Grid
//{
//
//	template<typename T>
//	static Array2D<T> BoxBlur(const UnlimitedArray2D<T> &grid, int distance = 1, int iterations = 1)
//	{
//		Array2D<T> newGrid(grid.Length[0], grid.Length[1]);
//
//		for(int i = 0; i < iterations; i++)
//		{
//			grid.ForAll([&grid, &newGrid, distance](int x, int y)
//				{
//					T result = T();
//					result += grid(x - distance, y);     //W
//					result += grid(x - distance, y - distance); //SW
//					result += grid(x, y - distance);     //S
//					result += grid(x + distance, y - distance); //SE
//					result += grid(x + distance, y);     //E
//					result += grid(x + distance, y + distance); //NE
//					result += grid(x, y + distance);     //N
//					result += grid(x - distance, y + distance); //NW
//
//					newGrid(x, y) = result / 8.f;
//				});
//		}
//
//		return newGrid;
//	}
//
//	template<typename O, typename I>
//	static Array2D<O> Cast(const Array2D<I> &grid)
//	{
//		Array2D<O> out(grid.Length[0], grid.Length[1]);
//		for(uint i = 0; i < grid.v.size(); i++) { out.v[i] = (O)grid.v[i]; }
//
//		return out;
//	}
//
//	template<typename T>
//	static IntVector2 Centroid(const std::vector<IntVector2> &collection)
//	{
//		IntVector2 result;
//		for(int i = 0; i < collection.size(); i++) { result += collection[i]; }
//
//		return IntVector2(result.x / collection.size(), result.y / collection.size());
//	}
//
//	/// <summary> Creates a grid with dimensions width and height, with each value generated by func. </summary>
//	/// <param name="func"> The function with which to generate the grid. Signature: (int, int)->T </param>
//	template<typename T, typename Func>
//	static Array2D<T> Create(uint width, uint height, Func func)
//	{
//		Array2D<T> grid(width, height);
//		grid.ForAll([&grid, func](int x, int y) { grid(x, y) = func(x, y); });
//
//		return grid;
//	}
//
//	//See https://stackoverflow.com/questions/14465297/connected-component-labelling for more information.
//#pragma region Connected Component Labelling
//	namespace 
//	{
//		/*
//		static void LabelCell(const Array2D<T> &grid, Array2D<int> &labels, std::deque<std::tuple<short, short>> &stack, std::function<bool(float)> getCondition, int startX, int startY, int current_label)
//		{
//			stack.push_front({ startX, startY });
//			while(!stack.empty())
//			{
//				short x = std::get<0>(stack.front()), y = std::get<1>(stack.front());
//				stack.pop_front();
//
//				if(x < 0 || x == grid.Length[0]) continue; // out of bounds
//				if(y < 0 || y == grid.Length[1]) continue; // out of bounds
//				if(labels(x, y) || !getCondition(grid(x, y))) continue; // already labeled or not marked with 1 in m
//
//				// mark the current cell
//				labels(x, y) = current_label;
//
//				stack.push_front({x - 1, y});
//				stack.push_front({x, y - 1});
//				stack.push_front({x + 1, y});
//				stack.push_front({x, y + 1});
//			}
//		}
//		*/
//
//		template<typename T, typename Func >
//		static void LabelCellRecursive(const Array2D<T> &grid, Array2D<int> &labels, Func fullCondition, int x, int y, int current_label)
//		{
//			if(x < 0 || x == grid.Length[0] || y < 0 || y == grid.Length[1]) return; // out of bounds
//			if(labels(x, y) || !fullCondition(grid(x, y))) return; // already labeled or not marked with 1 in m
//
//			// mark the current cell
//			labels(x, y) = current_label;
//
//			// recursively mark the neighbors
//			LabelCellRecursive(grid, labels, fullCondition, x - 1, y, current_label);
//			LabelCellRecursive(grid, labels, fullCondition, x, y - 1, current_label);
//			LabelCellRecursive(grid, labels, fullCondition, x + 1, y, current_label);
//			LabelCellRecursive(grid, labels, fullCondition, x, y + 1, current_label);
//		}
//	}
//
//	struct CCLOut { std::vector<std::vector<IntVector2>> ConnectedComponents; Array2D<int> LabelledGrid; };
//
//	/// <param name="fullCondition"> A function to know what cells count as empty/full. Signature: T->bool </param>
//	template<typename T, typename Func>
//	static CCLOut ConnectedComponentLabelling(const Array2D<T> &grid, bool generateCCL = false, Func fullCondition = [](T f) { return f; })
//	{
//		CCLOut out;
//		auto &LabelledGrid = out.LabelledGrid;
//		std::deque<std::tuple<short, short>> stack;
//
//		//LabelledGrid
//		LabelledGrid = Array2D<int>(grid.Length[0], grid.Length[1]);
//		int component = 0;
//		for(int x = 0; x < grid.Length[0]; x++)
//		{
//			for(int y = 0; y < grid.Length[1]; y++)
//			{
//				if(!LabelledGrid(x, y) && fullCondition(grid(x, y))) LabelCellRecursive(grid, LabelledGrid, fullCondition, x, y, ++component);
//			}
//		}
//
//		if(!generateCCL) { return out; }
//
//		//CCL
//		for(int x = 0; x < grid.Length[0]; x++)
//		{
//			for(int y = 0; y < grid.Length[1]; y++)
//			{
//				if(!LabelledGrid(x, y) || !fullCondition(grid(x, y))) { continue; }
//
//				int label = LabelledGrid(x, y);
//				if(out.ConnectedComponents.size() < label)
//				{
//					out.ConnectedComponents.resize(label);
//				}
//
//				out.ConnectedComponents[label - 1].push_back(IntVector2(x, y));
//			}
//		}
//
//		return out;
//	}
//#pragma endregion
//
//#pragma region Distance Transform and Voronoi
//	//TODO: Voronoi doesn't work
//	template<typename T, typename Func>
//	static Array2D<T> DistanceTransform(const Array2D<T> &grid, Array2D<IntVector2> &voronoiGrid = Array2D<IntVector2>::Null(), Func fullCondition = [&grid](float f) { return f > 0.f; })
//	{
//		return VoronoiDiagram::DistanceTransform(grid, voronoiGrid, fullCondition);
//	}
//
//	//See https://github.com/983/df (code) and http://fab.cba.mit.edu/classes/S62.12/docs/Meijster_distance.pdf (algorithm) for more information.
//
//	namespace
//	{
//		class VoronoiDiagram
//		{
//			template<typename T>
//			static T Distance(T x0, T dy0, T x)
//			{
//				T dx0 = x - x0;
//				return dx0 * dx0 + dy0 * dy0;
//			}
//
//		public:
//			template<typename T, typename Func>
//			static Array2D<T> DistanceTransform(const Array2D<T> &grid, Array2D<IntVector2> &closestPoints, Func fullCondition)
//			{
//				int x, y, width = grid.Length[0], height = grid.Length[1];
//
//				//There are at most n parabolas and n + 1 intersections between consecutive parabolas.
//				std::vector<int> parabolaX(width);
//				std::vector<T> xIntersections(width);
//				std::vector<T> tempRowDistances(width);
//				std::vector<IntVector2> tempClosest(width);
//
//				bool pointsIsNull = Array2D<IntVector2>::IsNull(closestPoints);
//
//				if(!pointsIsNull)
//				{
//					closestPoints = Array2D<IntVector2>(width, height);
//					for(y = 0; y < height; y++)
//					{
//						for(x = 0; x < width; x++)
//						{
//							closestPoints(x, y) = IntVector2(x, y);
//						}
//					}
//				}
//
//				Array2D<T> distances = Create<T>(grid.Length[0], grid.Length[1], [&grid, fullCondition](int x, int y) { return fullCondition(grid(x, y)) ? 0 : INFINITY; });
//				//Find minimum distance in columns.
//				for(x = 0; x < width; x++)
//				{
//					for(y = 1; y < height; y++)
//					{
//						if(distances(x, y) > distances(x, y - 1) + 1.f)
//						{
//							distances(x, y) = distances(x, y - 1) + 1.f;
//							if(!pointsIsNull) closestPoints(x, y) = closestPoints(x, y - 1);
//						}
//					}
//
//					for(y = height - 2; y >= 0; y--)
//					{
//						if(distances(x, y) > distances(x, y + 1) + 1.f)
//						{
//							distances(x, y) = distances(x, y + 1) + 1.f;
//							if(!pointsIsNull) closestPoints(x, y) = closestPoints(x, y + 1);
//						}
//					}
//				}
//
//				//Find minimum distance in rows by finding lower envelope of parabolas.
//				for(y = 0; y < height; y++)
//				{
//					int n = 0;
//					parabolaX[0] = 0;
//
//					//Find first parabola with finite distance.
//					for(x = 0; x < width; x++)
//					{
//						if(distances(x, y) < INFINITY)
//						{
//							parabolaX[0] = x;
//							break;
//						}
//					}
//
//					for(x = x + 1; x < width; x++)
//					{
//						//Load  parabola (x1, dy1).
//						float dy1 = distances(x, y);
//
//						//Infinite parabolas are not part of the lower envelope.
//						if(dy1 == INFINITY) continue;
//
//						while(true)
//						{
//							//Load old parabola (x0, dy0).
//							int x0 = parabolaX[n];
//							float dy0 = distances(x0, y);
//
//							//If the old parabola (x0, dy0) is above the parabola (x1, dy1) at the point of the last intersection
//							if(n > 0 &&
//								Distance<T>((T)x0, (T)dy0, (T)xIntersections[n - 1]) >
//								Distance<T>((T)x, (T)dy1, (T)xIntersections[n - 1]))
//							{
//								//it will not be in the lower envelope and is discarded.
//								n--;
//							}
//							else
//							{
//								//Otherwise, add  parabola and its intersection.
//								xIntersections[n] = 0.5f / (x - x0) * (x * x + dy1 * dy1 - x0 * x0 - dy0 * dy0);
//								n++;
//								parabolaX[n] = x;
//								break;
//							}
//						}
//					}
//
//					//Fill in distance values based on lower envelopes
//					for(x = width - 1; x >= 0; x--)
//					{
//						int x0;
//						T dy0;
//						//Go to next parabola.
//						while(n > 0 && x < xIntersections[(decltype(xIntersections)::size_type)n - 1]) n--;
//
//						x0 = parabolaX[n];
//						dy0 = distances(x0, y);
//						//Can not write directly because we also read from same array.
//						tempRowDistances[x] = Distance<T>((T)x0, (T)dy0, (T)x);
//						if(!pointsIsNull) { tempClosest[x] = closestPoints(x0, y); }
//					}
//
//					//Copy back temporary values.
//					for(x = 0; x < width; x++)
//					{
//						distances(x, y) = tempRowDistances[x];
//						if(!pointsIsNull) closestPoints(x, y) = tempClosest[x];
//					}
//				}
//
//				return distances;
//			}
//		};
//	}
//#pragma endregion
//
//	template<typename T>
//	static Array2D<T> DrawCircle(const Array2D<T> &grid, int cx, int cy, float radius, T fillValue)
//	{
//		Array2D<T> result(grid);
//
//		IntVector2 minP(std::max<int>(cx - radius + 1, 0), std::max<int>(cy - radius + 1, 0));
//		IntVector2 maxP(std::min<int>(cx + radius - 1, grid.Length[0] - 1), std::min<int>(cy + radius - 1, grid.Length[1] - 1));
//
//		for(int y = minP.y; y <= maxP.y; y++)
//		{
//			for(int x = minP.x; x <= maxP.x; x++)
//			{
//				if(((x - cx) * (x - cx) + (y - cy) * (y - cy)) <= radius * radius - 1.f) { result(x, y) = fillValue; }
//			}
//		}
//
//		return result;
//	}
//
//	//TODO: Maybe https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4813928/ is faster?
//	//TODO: Understand and personalize this code!
//	// This is a conversion to C# of the algorithm which is implemented at:
//	// https://www.eriksmistad.no/moore-neighbor-contour-tracing-algorithm-in-c/
//	// http://www.imageprocessingplace.com/downloads_V3/root_downloads/tutorials/contour_tracing_Abeer_George_Ghuneim/moore.html
//	// This gets all boundaries in the given pixels.
//	// It assumes you're looking for boundaries between non-transparent shapes on a transparent background
//	// (using the isTransparent property);
//	// but you could modify this, to pass in a predicate to say what background color you're looking for (e.g. White).
//	/*static std::vector<std::vector<IntVector2>> FindBorders(std::vector<float> grid, float threshold = 0f, bool bordersAreSolid = true)
//	{
//		IntVector2 size =  IntVector2(grid.Length, grid[0].Length);
//		HashSet<IntVector2> found =  HashSet<IntVector2>();
//		std::vector<IntVector2> list = null;
//		std::vector<std::vector<IntVector2>> lists =  std::vector<std::vector<IntVector2>>();
//		bool inside = false;
//
//		// Defines the neighborhood offset position from current position and the neighborhood
//		// position we want to check next if we find a  border at checkLocationNr.
//		int width = size.x;
//		auto neighborhood =  Tuple<Func<IntVector2, IntVector2>, int>[]
//		{
//			 Tuple<Func<IntVector2, IntVector2>, int>(point = >  IntVector2(point.x - 1, point.y), 7),
//				 Tuple<Func<IntVector2, IntVector2>, int>(point = >  IntVector2(point.x - 1, point.y - 1), 7),
//				 Tuple<Func<IntVector2, IntVector2>, int>(point = >  IntVector2(point.x, point.y - 1), 1),
//				 Tuple<Func<IntVector2, IntVector2>, int>(point = >  IntVector2(point.x + 1, point.y - 1), 1),
//				 Tuple<Func<IntVector2, IntVector2>, int>(point = >  IntVector2(point.x + 1, point.y), 3),
//				 Tuple<Func<IntVector2, IntVector2>, int>(point = >  IntVector2(point.x + 1, point.y + 1), 3),
//				 Tuple<Func<IntVector2, IntVector2>, int>(point = >  IntVector2(point.x, point.y + 1), 5),
//				 Tuple<Func<IntVector2, IntVector2>, int>(point = >  IntVector2(point.x - 1, point.y + 1), 5)
//		};
//
//		for(int y = 0; y < size.y; ++y)
//		{
//			for(int x = 0; x < size.x; ++x)
//			{
//				IntVector2 point =  IntVector2(x, y);
//				// Scan for non-transparent pixel
//				if(found.Contains(point) && !inside)
//				{
//					// Entering an already discovered border
//					inside = true;
//					continue;
//				}
//				bool isSolid = grid[point.x][point.y] >= threshold;
//				if(isSolid && inside)
//				{
//					// Already discovered border point
//					continue;
//				}
//				if(!isSolid && inside)
//				{
//					// Leaving a border
//					inside = false;
//					continue;
//				}
//				if(isSolid && !inside)
//				{
//					lists.Add(list =  std::vector<IntVector2>());
//
//					// Undiscovered border point
//					found.Add(point); list.Add(point);  // Mark the start pixel
//					int checkLocationNr = 1;            // The neighbor number of the location we want to check for a  border point
//					IntVector2 startPos = point;        // Set start position
//					int counter = 0;                    // Counter is used for the jacobi stop criterion
//					int counter2 = 0;                   // Counter2 is used to determine if the point we have discovered is one single point
//
//					// Trace around the neighborhood
//					while(true)
//					{
//						// The corresponding absolute array address of checkLocationNr
//						IntVector2 checkPosition = neighborhood[checkLocationNr - 1].Item1(point);
//						// autoiable that holds the neighborhood position we want to check if we find a  border at checkLocationNr
//						int CheckLocationNr = neighborhood[checkLocationNr - 1].Item2;
//
//						bool outsideGrid = (checkPosition.x >= size.x || checkPosition.y >= size.y);
//						if(bordersAreSolid) { isSolid = outsideGrid || grid[checkPosition.x][checkPosition.y] >= threshold; }
//						else { isSolid = !outsideGrid && grid[checkPosition.x][checkPosition.y] >= threshold; }
//
//						if(isSolid)
//						{
//							// Next border point found
//							if(checkPosition == startPos)
//							{
//								counter++;
//
//								// Stopping criterion (jacob)
//								if(CheckLocationNr == 1 || counter >= 3)
//								{
//									// Close loop
//									inside = true; // Since we are starting the search at were we first started we must set inside to true
//									break;
//								}
//							}
//
//							checkLocationNr = CheckLocationNr; // Update which neighborhood position we should check next
//							point = checkPosition;
//							counter2 = 0;             // Reset the counter that keeps track of how many neighbors we have visited
//							found.Add(point); list.Add(point); // Set the border pixel
//						}
//						else
//						{
//							// Rotate clockwise in the neighborhood
//							checkLocationNr = 1 + (checkLocationNr % 8);
//							if(counter2 > 8)
//							{
//								// If counter2 is above 8 we have traced around the neighborhood and
//								// therefor the border is a single black pixel and we can exit
//								counter2 = 0;
//								list = null;
//								break;
//							}
//							else
//							{
//								counter2++;
//							}
//						}
//					}
//
//				}
//			}
//		}
//		return lists;
//	}
//
//	// This gets the longest boundary (i.e. list of points), if you don't want all boundaries.
//	static std::vector<IntVector2> getPoints(std::vector<std::vector<IntVector2>> lists)
//	{
//		lists.Sort((x, y) = > x.Count.CompareTo(y.Count));
//		return lists.Last();
//	}
//
//	*/
//#pragma region FloodFill
//	//See http://will.thimbleby.net/scanline-flood-fill/ for more information.
//
//	//TODO: Fix broken FloodFill (or delete if unnecessary)
//	/// <summary>
//	/// Flood fills a grid starting from cell, and replaces all grid cells with replacementValue.
//	/// </summary>
//	/// <param name="cell">The cell to start filling at.</param>
//	/// <param name="replacementValue">The value to replace filled grid points with.</param>
//	/// <param name="fillCondition">The function to evaluate whether to fill a cell or not. By default it evaluates whether the grid cell equals the targetValue.</param>
//	/// <param name="targetValue">The value to compare to the grid number as the 3rd parameter of fillCondition. By default it takes the starting cell's value.</param>
//	template<typename T>
//	static Array2D<T> FloodFill(Array2D<T> grid, int x, int y, T replacementValue, T targetValue = 0, std::function<bool(T, T)> fillCondition = [](int a, int b) { return a == b; })
//	{
//		if(targetValue == 0) { targetValue = grid(x, y); }
//		if(grid(x, y) == replacementValue) { return grid; }
//
//		std::stack<IntVector2> cells;
//
//		cells.push(IntVector2(x, y));
//		while(cells.size() != 0)
//		{
//			IntVector2 temp = cells.top();
//			cells.pop();
//			int x1 = temp.x;
//			int y1 = temp.y;
//			while(y1 >= 0 && fillCondition(grid(x1, y1), targetValue)) { y1--; }
//			y1++;
//			bool spanLeft = false;
//			bool spanRight = false;
//			while(y1 < grid.Length[1] && fillCondition(grid(x1, y1), targetValue))
//			{
//				grid(x1, y1) = replacementValue;
//
//				if(!spanLeft && x1 > 0 && fillCondition(grid(x1 - 1, y1), targetValue))
//				{
//					cells.push(IntVector2(x1 - 1, y1));
//					spanLeft = true;
//				}
//				else if(spanLeft && x1 - 1 == 0 && !fillCondition(grid(x1 - 1, y1), targetValue))
//				{
//					spanLeft = false;
//				}
//				if(!spanRight && x1 < grid.Length[0] - 1 && fillCondition(grid(x1 + 1, y1), targetValue))
//				{
//					cells.push(IntVector2(x1 + 1, y1));
//					spanRight = true;
//				}
//				else if(spanRight && x1 < grid.Length[0] - 1 && !fillCondition(grid(x1 + 1, y1), targetValue))
//				{
//					spanRight = false;
//				}
//				y1++;
//			}
//		}
//
//		return grid;
//	}
//
//#pragma endregion
//
//	template<typename T>
//	static Array2D<T> Lerp(const Array2D<T> &a, const Array2D<T> &b, float t)
//	{
//		assert(a.Length[0] == b.Length[0] && a.Length[1] && b.Length[1]);
//		return Create<T>(a.Length[0], a.Length[1], [&a, &b, t](int x, int y) { return a(x, y) * (1 - t) + b(x, y) * t; });
//	}
//
//	/// <summary> Linearly interpolates grid with another grid generated by func. </summary>
//	/// <param name="func"> The function to generate the second grid with which to interpolate. Signature: (int, int)->T </param>
//	/// <param name="strength"> How much of an influence the function genereated grid has on the result. 0: original grid; 1: generated grid </param>
//	template<typename T, typename Func >
//	static Array2D<T> Manipulate(const Array2D<T> &grid, Func func, float strength)
//	{
//		Array2D<T> result(grid.Length[0], grid.Length[1]);
//		result = Create<T>(grid.Length[0], grid.Length[1], func);
//		result = Lerp(grid, result, strength);
//
//		return grid;
//	}
//
//	/// <summary> Masks the given grid, conserving the original values of the grid contained in conservedCells, and setting all others to maskValue. </summary>
//	template<typename T>
//	static Array2D<T> Mask(const Array2D<T> &grid, const std::vector<IntVector2> &conservedCells, float maskValue = 0)
//	{
//		Array2D<T> result(grid.Length[0], grid.Length[1]);
//		if(maskValue != 0) { std::fill(result.v.begin(), result.v.end(), maskValue); }
//		//for(auto iv : conservedCells) { result(iv.x, iv.y) = grid(iv.x, iv.y); }
//
//		return result;
//	}
//
//	/// <summary>
//	/// Masks the given grid, setting maskValue for cells contained in maskedCells, and setting all others to the origianl values.
//	/// </summary>
//	template<typename T>
//	static Array2D<T> MaskSubtract(const Array2D<T> &grid, const std::vector<IntVector2> &maskedCells, float maskValue = 0)
//	{
//		Array2D<T> result = grid;
//		for(auto iv : maskedCells) { result(iv.x, iv.y) = maskValue; }
//
//		return result;
//	}
//
//	/// <summary>
//	/// Masks the given grid, setting fillValue for cells contained in conservedCells, and setting all others to maskValue.
//	/// </summary>
//	template<typename T>
//	static Array2D<T> MaskFilled(const Array2D<T> &grid, const std::vector<IntVector2> &conservedCells, float fillValue = 1, float maskValue = 0)
//	{
//		Array2D<T> result(grid.Length[0], grid.Length[1]);
//		if(maskValue != 0) { std::fill(result.v.begin(), result.v.end(), maskValue); }
//		for(auto iv : conservedCells) { result(iv.x, iv.y) = fillValue; }
//
//		return result;
//	}
//
//	//TODO: Make NearestPointsInCollection not neccessarily have to create a Voronoi diagram, if the user already has one. Problem: The user doesn't know which collection's Voronoi diagram is needed.
//	//TODO: Make NearestPointsInCollection work on a grid the size of the bounding box of collectionA and collectionB, instead of the whole grid they are based on (and check whether it's more efficient!).
//	//TODO: Make Mask's maskValue and the DistanceTransform's minValueThreshold be non-hard-coded (is this really neccessary? <- Neo from the far future).
//
//	//This algorithm works by getting the Voronoi diagram of the larger collectionA,
//	//looking at all the Voronoi regions that touch collectionB, and comparing (brute-force)
//	//all those regions' seeds* to all points of collectionB to find the smallest distance.
//	//See Vobarkun's answer on https://math.stackexchange.com/questions/2399957/shortest-distance-between-two-digital-blobs for more information.
//	//*seed: the point a Voronoi region refers to. See https://en.wikipedia.org/wiki/Voronoi_diagram.
//
//	/// <summary>
//	/// Returns the two closest points that are from two different collections of points.
//	/// </summary>
//	static std::array<IntVector2, 2> NearestPointsInCollections(int gridWidth, int gridHeight, std::vector<IntVector2> collectionA, std::vector<IntVector2> collectionB)
//	{//TODO: Voronoi: Add getCondition
//		//Voronoi Diagram
//		Array2D<IntVector2> voronoi;
//		//We should create the Voronoi diagram of the largest collection of the two, as the Voronoi's efficiency
//		//is mostly independant on the collection's size, the brute-force comparison afterwards isn't.
//		if(collectionA.size() < collectionB.size())
//		{
//			std::vector<IntVector2> collectionTemp = collectionA;
//			collectionA = collectionB;
//			collectionB = collectionTemp;
//		}
//
//		//Create a grid with only collectionA and get it's Voronoi diagram.
//		Array2D<float> collectionAGrid(gridWidth, gridHeight);
//		collectionAGrid = Mask(collectionAGrid, collectionA, -2);
//
//		DistanceTransform(collectionAGrid, voronoi, [&collectionAGrid](float f) { return f > -1.f; });
//
//		//Find all regions of the diagram, which contain seeds* that are possible nearest points of collectionB.
//		std::vector<IntVector2> closeSeeds;
//		for(int i = 0; i < collectionB.size(); i++)
//		{
//			IntVector2 collectionPoint = collectionB[i];
//			IntVector2 closeSeed = voronoi(collectionPoint.x, collectionPoint.y);
//			if(std::find(closeSeeds.begin(), closeSeeds.end(), closeSeed) != closeSeeds.end())
//			{
//				closeSeeds.push_back(closeSeed);
//			}
//		}
//
//		//Brute-force the distance between all points of collectionB and the remaining points of collectionA.
//		float minDistance = INFINITY;
//		int nearestIA = -1, nearestIB = -1;
//		for(int iB = 0; iB < collectionB.size(); iB++)
//		{
//			IntVector2 b = collectionB[iB];
//			for(int iA = 0; iA < closeSeeds.size(); iA++)
//			{
//				IntVector2 a = closeSeeds[iA];
//				IntVector2 diff = b - a;
//				float Distance = (float)(diff.x * diff.x + diff.y * diff.y);
//
//				if(Distance < minDistance)
//				{
//					minDistance = Distance;
//					nearestIA = iA;
//					nearestIB = iB;
//				}
//			}
//		}
//		return { IntVector2(), IntVector2() };
//		//return { closeSeeds[nearestIA], collectionB[nearestIB] };
//	}
//
//#undef min
//#undef max
//	template<typename T>
//	static Array2D<T> Normalize(const Array2D<T> &grid, T maxValue = 1.f)
//	{
//		T max = std::numeric_limits<T>::min();
//		for(uint i = 0; i < grid.v.size(); i++)
//		{
//			max = (grid.v[i] > max) ? grid.v[i] : max;
//		}
//
//		return Create<T>(grid.Length[0], grid.Length[1], [&grid, max](int x, int y) { return grid(x, y) / max; });
//	}
//
//	//See https://rosettacode.org/wiki/Bilinear_interpolation for more information.
//	template<typename T>
//	static Array2D<T> ScaleBy(const ClampedArray2D<T> &grid, float scaleX, float scaleY)
//	{
//		int newWidth = (int)grid.Length[0] * scaleX;
//		int newHeight = (int)grid.Length[1] * scaleY;
//
//		Array2D<T> result(newWidth, newHeight);
//
//		for(uint y = 0; y < newHeight; y++)//1: Make changes here
//		{
//			for(uint x = 0; x < newWidth; x++)//1: Make changes here
//			{
//				float gx = x / (float)(newWidth) * (grid.Length[0]);//2: Make changes here
//				float gy = y / (float)(newHeight) * (grid.Length[1]);//2: Make changes here
//
//				int gxi = (int)gx;
//				int gyi = (int)gy;
//				T c00 = grid(gxi, gyi);
//				T c10 = grid(gxi + 1, gyi);	/*TODO: Currently, ScaledBy has to sample 1 pixel over the right/upper border of the grid, is there any way of preventing this? (see comment 1 and 2)*/
//				T c01 = grid(gxi, gyi + 1);
//				T c11 = grid(gxi + 1, gyi + 1);
//				result(x, y) = Math::BiLerp(c00, c10, c01, c11, gx - gxi, gy - gyi);
//			}
//		}
//
//		return result;
//	}
//
//	template<typename T>
//	static Array2D<T> ScaleBy(const Array2D<T> &grid, float scale) { return ScaleBy<T>(grid, scale, scale); }
//	template<typename T>
//	static Array2D<T> ScaleTo(const Array2D<T> &grid, int width, int height)
//	{
//		float scaleX = (float)width / grid.Length[0];
//		float scaleY = (float)height / grid.Length[1];
//
//		return ScaleBy<T>(grid, scaleX, scaleY);
//	}
//};
//
