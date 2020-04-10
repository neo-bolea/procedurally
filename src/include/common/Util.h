#pragma once

#include "Common/Preprocessor.h"

#include <type_traits>

namespace ext
{
	template<typename Container, typename Key>
	bool contains(const Container &cont, const Key &toFind)
	{
		return std::find(cont.begin(), cont.end(), toFind) == cont.end();
	}

	/*template<typename Container, typename Key, typename Value>
	bool find(
		const Container &cont, 
		const Key &toFind, 
		Value &result)
	{
		auto &it = std::find(cont.begin(), cont.end(), toFind);
		if (it != cont.end())
		{
			result = it->second;
			return true;
		}
		return false;
	}*/

	/*template<typename Value, typename From>
	const Value &retrieve(const From &from)
	{
		if constexpr (std::is_convertible_v<From, Value>)
		{
			return from;
		}
		else if constexpr (std::is_convertible_v<decltype(*from), Value>)
		{
			return *from;
		}
		else
		{
			return from->second;
		}
	}*/
}

#pragma region 2D/3D Sliced For Loops

#define LeftDecl   xStart = 0;          xEnd = 1;
#define RightDecl  xStart = _xSize - 1; xEnd = _xSize;
#define BottomDecl yStart = 0;          yEnd = 1;
#define TopDecl    yStart = _ySize - 1; yEnd = _ySize;
#define BackDecl   zStart = 0;          zEnd = 1;
#define FrontDecl  zStart = _zSize - 1; zEnd = _zSize;

enum class CubeSlices2D
{
	Center,

	//Edges
	Left, Right,
	Bottom, Top,

	//Corners
	BottomLeft, BottomRight, TopLeft, TopRight,

	MAX
};

#define SLICED_LOOP_2D(__xSize, __ySize) 	int _xSize = __xSize, _ySize = __ySize;	\
{																												\
	for(uint _side = 0; _side <= (uint)CubeSlices2D::MAX; _side++)							\
	{																											\
		int xStart = 1, xEnd = _xSize - 1, yStart = 1, yEnd = _ySize - 1;					\
																												\
		switch(_side)																						\
		{																										\
		case (uint)CubeSlices2D::Center			: break;											\
																												\
		case (uint)CubeSlices2D::Left				: LeftDecl break;								\
		case (uint)CubeSlices2D::Right			: RightDecl break;							\
						  		 																				\
		case (uint)CubeSlices2D::Bottom			: BackDecl break;								\
		case (uint)CubeSlices2D::Top				: FrontDecl break;							\
																												\
		case (uint)CubeSlices2D::BottomLeft		: BottomDecl LeftDecl break;				\
		case (uint)CubeSlices2D::BottomRight	: BottomDecl RightDecl break;				\
		case (uint)CubeSlices2D::TopLeft			: TopDecl LeftDecl break;					\
		case (uint)CubeSlices2D::TopRight		: TopDecl RightDecl break;					\
		}																										\
																												\
																												\
		const bool left   = (_side == 1 ||	_side == 5 || _side == 7),						\
					  right  = (_side == 2 ||	_side == 6 || _side == 8),						\
					  bottom = (_side == 3 ||	_side == 5 || _side == 6),						\
					  top    = (_side == 4 ||	_side == 7 || _side == 8),						\
					  center = (_side == 0);															\
																												\
		for(uint x = xStart; x < xEnd; x++)															\
		{																										\
			for(uint y = yStart; y < yEnd; y++)														\
			{
#define SLICED_LOOP_2D_END }}}}

enum class CubeSlices3D
{
	Center,

	//Faces
	Left, Right,
	Bottom, Top,
	Back, Front,

	//Edges
	BackLeft, LeftFront, FrontRight, RightBack,
	BackBottom, BottomFront, FrontTop, TopBack,
	LeftBottom, BottomRight, RightTop, TopLeft,

	//Corners
	BackBottomLeft, BackBottomRight, BackTopLeft, BackTopRight,
	FrontBottomLeft, FrontBottomRight, FrontTopLeft, FrontTopRight,

	MAX
};

#define SLICED_LOOP_3D(__xSize, __ySize, __zSize) 	int _xSize = __xSize, _ySize = __ySize, _zSize = __zSize;								                                 \
{																																																							\
	for(uint _side = 0; _side <= (uint)CubeSlices3D::MAX; _side++)																											                     \
	{																																											                                 \
		int xStart = 1, xEnd = _xSize - 1, yStart = 1, yEnd = _ySize - 1, zStart = 1, zEnd = _zSize - 1;										                                 \
																																												                                 \
		switch(_side)																																							                              \
		{																																										                                 \
		case (uint)CubeSlices3D::Center             : break;																											                           \
																																											                                    \
		case (uint)CubeSlices3D::Left				     : LeftDecl break;																								                           \
		case (uint)CubeSlices3D::Right				  : RightDecl break;																								                           \
			 					  		 																																		                                 \
		case (uint)CubeSlices3D::Bottom			     : BottomDecl break;																							                           \
		case (uint)CubeSlices3D::Top				     : TopDecl break;																								                           \
						  		 																																		                                       \
		case (uint)CubeSlices3D::Back				     : BackDecl break;																								                           \
		case (uint)CubeSlices3D::Front				  : FrontDecl break;																								                           \
	                                                                                                                                                                  \
																																								                                             \
		case (uint)CubeSlices3D::BackLeft			  : BackDecl LeftDecl break;																					                           \
		case (uint)CubeSlices3D::LeftFront		     : LeftDecl FrontDecl break;																					                           \
		case (uint)CubeSlices3D::FrontRight		     : FrontDecl RightDecl break;																				                           \
		case (uint)CubeSlices3D::RightBack		     : RightDecl BackDecl break;																					                           \
			 		  																																				                                          \
		case (uint)CubeSlices3D::BackBottom		     : BackDecl BottomDecl break;																				                           \
		case (uint)CubeSlices3D::BottomFront		  : BottomDecl FrontDecl break;																				                           \
		case (uint)CubeSlices3D::FrontTop			  : FrontDecl TopDecl break;																					                           \
		case (uint)CubeSlices3D::TopBack			     : TopDecl BackDecl break;																					                           \
				  																																				                                             \
		case (uint)CubeSlices3D::LeftBottom		     : LeftDecl BottomDecl break;																				                           \
		case (uint)CubeSlices3D::BottomRight		  : BottomDecl RightDecl break;																				                           \
		case (uint)CubeSlices3D::RightTop			  : RightDecl TopDecl break;																					                           \
		case (uint)CubeSlices3D::TopLeft			     : TopDecl LeftDecl break;																					                           \
			 			  																																				                                       \
			 			  																																				                                       \
		case (uint)CubeSlices3D::BackBottomLeft	  : BackDecl BottomDecl LeftDecl break;																	                           \
		case (uint)CubeSlices3D::BackBottomRight    : BackDecl BottomDecl RightDecl break;																	                           \
		case (uint)CubeSlices3D::BackTopLeft		  : BackDecl TopDecl LeftDecl break;																		                           \
		case (uint)CubeSlices3D::BackTopRight	     : BackDecl TopDecl RightDecl break;																		                           \
																																							                                                \
		case (uint)CubeSlices3D::FrontBottomLeft    : FrontDecl BottomDecl LeftDecl break;																	                           \
		case (uint)CubeSlices3D::FrontBottomRight   : FrontDecl BottomDecl RightDecl break;																	                           \
		case (uint)CubeSlices3D::FrontTopLeft	     : FrontDecl TopDecl LeftDecl break;																		                           \
		case (uint)CubeSlices3D::FrontTopRight	     : FrontDecl TopDecl RightDecl break;																		                           \
		}																																										                                 \
																																												                                 \
																																												                                 \
		const bool left   = (_side == 1 ||	_side == 7  || _side == 8  || _side == 15 || _side == 18 ||		_side == 19 || _side == 21 || _side == 23 || _side == 25),	\
					  right  = (_side == 2 ||	_side == 9  || _side == 10 || _side == 16 || _side == 17 ||		_side == 20 || _side == 22 || _side == 24 || _side == 26),	\
					  bottom = (_side == 3 ||	_side == 11 || _side == 12 || _side == 15 || _side == 16 ||		_side == 19 || _side == 20 || _side == 23 || _side == 24),	\
					  top    = (_side == 4 ||	_side == 13 || _side == 14 || _side == 17 || _side == 18 ||		_side == 21 || _side == 22 || _side == 25 || _side == 26),	\
					  back   = (_side == 5 ||	_side == 7  || _side == 10 || _side == 11 || _side == 14 ||		_side == 19 || _side == 20 || _side == 21 || _side == 22),	\
					  front  = (_side == 6 ||	_side == 8  || _side == 9  || _side == 12 || _side == 13 ||		_side == 23 || _side == 24 || _side == 25 || _side == 26),	\
					  center = (_side == 0);																																	                           \
																																												                                 \
		for(uint x = xStart; x < xEnd; x++)																															                                 \
		{																																										                                 \
			for(uint y = yStart; y < yEnd; y++)																														                                 \
			{																																									                                 \
				for(uint z = zStart; z < zEnd; z++)																													                                 \
				{
#define SLICED_LOOP_3D_END }}}}}

#pragma endregion