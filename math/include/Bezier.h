#include "Vec.h"

#include <assert.h>
#include <vector>

namespace Bezier
{
	fVec2 GetBezierPoint(fVec2 a, fVec2 b, fVec2 c, fVec2 d, float t)
	{
		float mt = 1 - t;
		float mt2 = mt * mt;
		float mt3 = mt2 * mt;
	
		float t2 = t * t;
		float t3 = t2 * t;
		return mt3 * a + 3 * t * mt2 * b + 3 * t2 * mt * c + t3 * d;
	}
	
	fVec2 GetSplinePoint(const std::vector<fVec2> &points, float t)
	{
		assert(t >= 0.f && t < 1.f, "t has to be inside the range [0, 1)."); 
		t *= ((points.size() - 1) / 3);
		int i = (int)t;
		t -= i;
		i *= 3;
		
		return GetBezierPoint(points[i], points[i + 1], points[i + 2], points[i + 3], t);
	}
	
	void GenerateSpline(const std::vector<fVec2> &ctrlPoints, std::vector<fVec2> &result, int resolution)
	{
		result.resize(resolution);
		double inc = 1.0 / resolution;
		double t = 0;
		for (int i = 0; i < resolution; i++)
		{
			result[i] = GetSplinePoint(ctrlPoints, t);
			t += inc;
		}
	}
}