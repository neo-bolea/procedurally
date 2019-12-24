#pragma region Includes
#define GLEW_STATIC
#define SDL_MAIN_HANDLED
#include "Camera.h"
#include "Core.h"
#include "GL.h"
#include "GLTypes.h"
#include "GPUGrid.h"
#include "Grid.h"
#include "MathExt.h"
#include "MathGL.h"
#include "Mesh.h"
#include "NTree.h"
#include "SDL.h"
#include "Shader.h"
#include "StringUtils.h"
#include "Systems/Inputs.h"
#include "Time.h"
#include "Watch.h"
#include "framework.h"
#include "pch.h"

#define STB_IMAGE_IMPLEMENTATION
#include "FastNoise.h"
#include "GL/glew.h"
#include "stb_image.h"

#include <charconv>
#include <fstream>
#include <iostream>
#include <random>
#pragma endregion

#define __WINDOWS_WASAPI__
#define __WINDOWS_DS__
#define __WINDOWS_ASIO__

#define WIDTH (1920 * 0.9)
#define HEIGHT (1080 * 0.9)
#define ASPECT ((float)WIDTH / HEIGHT)

using Point = dVec2;
using Shape = std::function<double(Point)>;

static Shape u(Shape a, Shape b)
{
	return [a, b](Point v) { return std::min(a(v), b(v)); };
}

static Shape n(Shape a, Shape b)
{
	return [a, b](Point v) { return std::max(a(v), b(v)); };
}

static Shape inv(Shape a)
{
	return [a](Point v) { return -a(v); };
}

struct Circle
{
	constexpr Circle(Point coords, double r) : coords(coords), r(r) {}
	Point coords; double r;
	double operator()(Point v) const { return sqrtl(std::powl(coords.x - v.x, 2) + std::powl(coords.y - v.y, 2)) - r; }
};

struct Left
{
	constexpr Left(Point v) : x0(v.x) {}
	double x0;
	constexpr double operator()(Point v) const { return v.x - x0; }
};

struct Right
{
	constexpr Right(Point v) : x0(v.x) {}
	double x0;
	constexpr double operator()(Point v) const { return x0 - v.x; }
};

struct Lower
{
	constexpr Lower(Point v) : y0(v.y) {}
	double y0;
	constexpr double operator()(Point v) const { return v.y - y0; }
};

struct Upper
{
	constexpr Upper(Point v) : y0(v.y) {}
	double y0;
	constexpr double operator()(Point v) const { return y0 - v.y; }
};

struct Rectangle
{
	Rectangle(Point min, Point max) : Min(min), Max(max) {}
	Point Min, Max;
	Shape func = n(n(Right(Min), Left(Max)), n(Upper(Min), Lower(Max)));
	double operator()(Point v) const { return func(v); }
};

Shape HShape =
u(Rectangle(Point(0.1, 0.1), Point(0.25, 0.9)),
	u(Rectangle(Point(0.45, 0.1), Point(0.6, 0.35)),
		n(Circle(Point(0.35, 0.35), 0.25),
			inv(u(Circle(Point(0.35, 0.35), 0.1),
				Rectangle(Point(0.25, 0.1), Point(0.45, 0.35))))
		)
	)
);

Shape IShape = u(Rectangle(Point(0.75, 0.1), Point(0.9, 0.55)), Circle(Point(0.825, 0.75), 0.1));

Shape HIShape = u(HShape, IShape);

template<typename Container>
void Triangulate(Container &cont, const fVec2 &v0, const fVec2 &v1)
{
	fVec3 vv0 = ((fVec3)v0 - fVec3::NoZ*0.5f) * 10;
	fVec3 vv1 = ((fVec3)v1 - fVec3::NoZ*0.5f) * 10;
	cont.push_back(vv0);
	cont.push_back(vv1);
}

float f(FastNoise fn, fVec3 coords)
{
	//return !(int(coords.x * 64) % 2 == 0 && int(coords.y * 64) % 2 == 0);
	//return std::max(std::max((0.2 - coords.x), (coords.x - 0.8)), std::max((0.2 - coords.y), (coords.y - 0.8)));
	return HIShape((Point)coords);
	fn.GradientPerturb(coords.x, coords.y, coords.z);
	return fn.GetNoise(coords.x, coords.y, coords.z);
}

fVec2 fD(FastNoise fn, fVec3 coords)
{
	float epsilon = 0.0001f;
	fVec2 d;
	d.x = f(fn, fVec3(coords.x + epsilon, coords.y, coords.z)) - f(fn, fVec3(coords.x - epsilon, coords.y, coords.z));
	d.y = f(fn, fVec3(coords.x, coords.y + epsilon, coords.z)) - f(fn, fVec3(coords.x, coords.y - epsilon, coords.z));
	return d.Normalize();
}


auto GenerateMarchingSquares(
	uint VAO[4], float noiseZ, float QEFEpsilon,
	std::vector<fVec3> &msEdges, 
	std::vector<fVec3> &emptyCells,
	std::vector<fVec3> &fullCells,
	std::vector<fVec3> &btwCells)
{
	struct BorderInfo
	{
		float Value;
		uchar Config;

		BorderInfo() : Value(-1), Config(-1) {}
		BorderInfo(float value, uchar config) : Value(value), Config(config) {}
		bool operator ==(const BorderInfo &other) { return Config == other.Config; }
		bool operator !=(const BorderInfo &other) { return !(*this == other); }

		BorderInfo operator +(const BorderInfo &other) { return BorderInfo((Value + other.Value) / 2., Config); }
		BorderInfo operator /(uint other) { return BorderInfo(Value / other, Config); }
	};

#ifdef NDEBUG
	Grid<float> gridTest(1024, 1024);
	NTree<BorderInfo, 10, CoordinateSystem::Normalized> tree(BorderInfo(0, 0));
#else
	Grid<float> gridTest(64, 64);
	NTree<BorderInfo, 6, CoordinateSystem::Normalized> tree(BorderInfo(0, 0));
#endif

	Watch watchTotal(Watch::ms);
	watchTotal.Start();
	//tree.Map([&gridTest](decltype(tree)::NodeInfo& info) { info.Node = HIShape(Point(info.Pos[0], info.Pos[1])) < 0; });
	FastNoise fn;
	fn.SetFractalType(FastNoise::FBM);
	fn.SetCellularReturnType(FastNoise::Distance);
	fn.SetCellularDistanceFunction(FastNoise::Euclidean);

	fn.SetFrequency(8);
	fn.SetGradientPerturbAmp(0.0);
	fn.SetFractalOctaves(4);
	fn.SetFractalLacunarity(2.0);
	fn.SetFractalGain(0.5);

	gridTest.ForAll([&gridTest, &fn, noiseZ](decltype(gridTest)::Coords coords)
		{ 
			fVec2 fCoords = fVec2(coords) / gridTest.Size[0];
			gridTest(coords) = f(fn, fVec3(fCoords.x, fCoords.y, noiseZ));
		});

	for (uint xx = 0; xx < gridTest.Size[0]; xx++)
	{
		for (uint yy = 0; yy < gridTest.Size[1]; yy++)
		{
			float cornerValue = gridTest(xx, yy);
			bool values[4] =
			{
				(cornerValue > 0),
				(gridTest(xx + 1, yy) > 0),
				(gridTest(xx + 1, yy + 1) > 0),
				(gridTest(xx, yy + 1) > 0),
			};
			int config = values[0] + (values[1] + (values[2] + (values[3] << 1) << 1) << 1);
			uchar value = 0;
			if (config)
			{
				value = 2;
				if (config == 15) { value = 1; }
			}

			tree.Set(fVec2(yy, xx) / gridTest.Size[0], { cornerValue, value });
		}

		if (xx % 60 == 0) { tree.Merge(); }
	}
	tree.Merge();

	msEdges.clear();
	emptyCells.clear();
	fullCells.clear();
	btwCells.clear();
	int nodeCount = tree.NodeCount();
	msEdges.reserve(nodeCount);
	emptyCells.reserve(nodeCount);
	fullCells.reserve(nodeCount);
	btwCells.reserve(nodeCount);

	struct VecHash
	{ size_t operator()(const iVec2 &v) const { return v.x * 8192 * 8192 + v.y; } };
	std::unordered_map<iVec2, fVec2, VecHash> edgePoints;

	//std::vector<decltype(tree)::NodeInfo> edgePoints;
	//std::vector<fVec2> solvePoints;
	//std::vector<uint> indices;
	//tree.GetConnectionGraph(edgePoints, indices);
	//for (size_t i = 0; i < vertices.size(); i += 2)
	//{
	//	auto &a = edgePoints.find(iVec2(vertices[i] * gridTest.Size[0]));
	//	auto &b = edgePoints.find(iVec2(vertices[i+1] * gridTest.Size[0]));
	//
	//	if(a != edgePoints.end() && b != edgePoints.end())
	//		Triangulate(msEdges, a->second, b->second);
	//}

	tree.Map([&](auto &info)
	//gridTest.ForAll([&](auto coords)
	//for (size_t i = 0; i < edgePoints.size(); i++)
		{
			//auto info = edgePoints[i];
			if (info.GetNode().Data.Config != 2) { return; }

			float size = info.Size;
			fVec2 pos = fVec2(info.Pos) + fVec2(info.Size / 2.f);
			fVec2
				BotLefCor = pos + fVec2(-size, -size) / 2,
				BotRigCor = pos + fVec2(size, -size) / 2,
				TopRigCor = pos + fVec2(size, size) / 2,
				TopLefCor = pos + fVec2(-size, size) / 2;

			std::array<float, 4> values =
			{
				f(fn, fVec3(BotLefCor.x, BotLefCor.y, noiseZ)/* + fVec3(0.0001f, 0.0001f, 0)*/),
				f(fn, fVec3(BotRigCor.x, BotRigCor.y, noiseZ)/* + fVec3(0.0001f, 0.0001f, 0)*/),
				f(fn, fVec3(TopRigCor.x, TopRigCor.y, noiseZ)/* + fVec3(0.0001f, 0.0001f, 0)*/),
				f(fn, fVec3(TopLefCor.x, TopLefCor.y, noiseZ)/* + fVec3(0.0001f, 0.0001f, 0)*/),
			};

			int config = (values[0] > 0) + ((values[1] > 0) + ((values[2] > 0) + ((values[3] > 0) << 1) << 1) << 1);
			if (config == 15 || config == 0) { return; }

			Triangulate(btwCells, BotLefCor, BotRigCor);
			Triangulate(btwCells, BotRigCor, TopRigCor);
			Triangulate(btwCells, TopRigCor, TopLefCor);
			Triangulate(btwCells, TopLefCor, BotLefCor);

			std::array<std::array<uchar, 2>, 4> indices =
			{
				std::array<uchar, 2>{ 0, 2 },
				std::array<uchar, 2>{ 0, 1 },
				std::array<uchar, 2>{ 1, 3 },
				std::array<uchar, 2>{ 2, 3 }
			};

			std::vector<fVec2> intersections, normals;

			for (size_t i = 0; i < 4; i++)
			{
				int c1 = indices[i][0];
				int c2 = indices[i][1];

				fVec2 rp1(c1 / 2, c1 % 2);
				fVec2 rp2(c2 / 2, c2 % 2);

				fVec2 ap1 = pos + ((rp1 - fVec2::One * 0.5) * size);
				fVec2 ap2 = pos + ((rp2 - fVec2::One * 0.5) * size);

				float v1 = f(fn, fVec3(ap1.x, ap1.y, noiseZ));
				float v2 = f(fn, fVec3(ap2.x, ap2.y, noiseZ));

				int cfg1 = (config >> c1) & 1;
				int cfg2 = (config >> c2) & 1;

				if ((v1 > 0) == (v2 > 0)) { continue; }

				float t = (-v1) / (v2 - v1);
				t = std::max(0.0001f, t);

				fVec2 intersection = rp1 + (rp2 - rp1) * t;
				fVec2 normalPos = pos + (intersection - fVec2::One * 0.5f) * size;
				fVec2 normal = fD(fn, fVec3(normalPos.x, normalPos.y, noiseZ));
				intersections.push_back(intersection);
				normals.push_back(normal);
			}
			if(normals.size() != 2){return;}
			auto qef = [&](fVec2 v)
			{
				float total = 0;
				for (size_t i = 0; i < intersections.size(); i++)
				{
					fVec2 rp = v - intersections[i];
					float dot = fVec2::Dot(normals[i], rp);
					total += dot * dot;
				}
				return total + (v - fVec2::One * 0.5).LenSqr() * 0.1f;
			};

			fVec2 solvePos(0., 0.);
			//float lr = 1.41421356;
			//for (size_t i = 0; i < 8; i++)
			//{
			//	lr /= 2.;
			//	fVec2 gradDir;
			//	gradDir.x = qef(solvePos + fVec2(0.001f, 0.f)) - qef(solvePos - fVec2(0.001f, 0.f));
			//	gradDir.y = qef(solvePos + fVec2(0.f, 0.001f)) - qef(solvePos - fVec2(0.f, 0.001f));
			//	solvePos = solvePos - gradDir.Normalize() * lr;
			//}

			fVec2 p1 = intersections[0], p3 = intersections[1];
			fVec2 n1 = normals[0], n2 = normals[1];
			fVec2 p2 = p1 + fVec2(n1.y, -n1.x), p4 = p3 + fVec2(n2.y, -n2.x);
			float t = (p1.x - p3.x)*(p3.y - p4.y)-(p1.y - p3.y)*(p3.x-p4.x)
				/ std::max(((p1.x - p2.x)*(p3.y - p4.y)-(p1.y - p2.y)*(p3.x-p4.x)), FLT_EPSILON);
			
			solvePos = p1 + (p2 - p1) * t;

			solvePos.x = Math::Clamp(solvePos.x, 0.f, 1.f);
			solvePos.y = Math::Clamp(solvePos.y, 0.f, 1.f);
			solvePos = pos + (solvePos - fVec2::One * 0.5) * size;
			//Triangulate(emptyCells, solvePos, solvePos + fVec2(0.0001f, 0.f));

			Triangulate(emptyCells, pos + (p1 - fVec2::One * 0.5) * size, pos + (p2 - fVec2::One * 0.5) * size + fVec2(0.0001f, 0.f));
			Triangulate(fullCells, pos + (p3 - fVec2::One * 0.5) * size, pos + (p4 - fVec2::One * 0.5) * size + fVec2(0.0001f, 0.f));


			//solvePoints.push_back(solvePos);
			edgePoints.insert({iVec2(info.Pos * gridTest.Size[0]), solvePos});
		});

	//std::vector<decltype(tree)::NodeInfo> connections;
	//tree.GetConnectionGraph(connections);
	//for (size_t i = 0; i < connections.size(); i += 2)
	//{
	//	//Triangulate(msEdges, connections[i].Pos, connections[i+1].Pos);
	//	auto &a = edgePoints.find(iVec2(connections[i].Pos * gridTest.Size[0]));
	//	auto &b = edgePoints.find(iVec2(connections[i+1].Pos * gridTest.Size[0]));
	//	
	//	//if(a != edgePoints.end() && b != edgePoints.end())
	//	//	Triangulate(msEdges, a->second, b->second);
	//}

	//for (size_t i = 0; i < solvePoints.size(); i += 2)
	//{
	//	Triangulate(msEdges, solvePoints[i], solvePoints[i + 1]);
	//}

	uint VAOEmpty;
	std::array<std::reference_wrapper<std::vector<fVec3>>, 4> containers = 
		{ emptyCells, fullCells, btwCells, msEdges };
	glGenVertexArrays(4, VAO);

	for (size_t i = 0; i < containers.size(); i++)
	{
		uint VBO;
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO[i]);

		GLHelper::SetVBOData(VBO, containers[i].get(), 3, 0);
	}

	Grid<float> gridFromTree(gridTest.Size[0], gridTest.Size[1]);
	gridFromTree.ForAll([&](decltype(gridFromTree)::Coords &coords)
		{ gridFromTree(coords) = tree.Get<false>(fVec2(coords[1], coords[0]) / gridTest.Size[0]).Value > 0; });

	watchTotal.Stop();
	std::cout << "Total Time: " << watchTotal.sTime() << std::endl;

	return gridFromTree;
}

int main(int argc, char *argv[])
{
#pragma region SDL
	// SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	// OpenGL 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// Anti-Aliasing
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	// Window
	auto window = SDL_CreateWindow("TestTitle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH,
		HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (window == NULL)
	{
		std::cout << "Failed to open an SDL window: " << std::endl << SDL_GetError() << std::endl;
		SDL_Quit();
		return EXIT_FAILURE;
	}

	auto context = SDL_GL_CreateContext(window);
#pragma endregion

#pragma region GLEW
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	// Culling
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);

	// Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Depth Test
	//glEnable(GL_DEPTH_TEST);

	// Multi Sampling
	glEnable(GL_MULTISAMPLE);

	// Errors
	glEnable(GL_DEBUG_OUTPUT);

	// VSync
	SDL_GL_SetSwapInterval(0);
#pragma endregion

	Inputs inputs;
	inputs.AddAxis("Horizontal", Inputs::E, Inputs::Q);
	inputs.AddAxis("Vertical",   Inputs::D, Inputs::A);
	inputs.AddAxis("Scroll",     Inputs::S, Inputs::W);
	Time time;

	bool quit = false;
	SDL_Event event;

	Time::SetLimitFPS(false);

	Camera cam(Vector3(0.f, 0.f, 10.f), Vector3::Forward);

	GL::ProgRef texProg = GL::Programs.Load({ "Min3D.vert", "Tex.frag" });
	GL::ProgRef solidProg = GL::Programs.Load({ "Min3D.vert", "Solid.frag" });
	std::array<fVec3, 4> triangles =
	{
		 fVec3(-0.5f, -0.5f, 0.0f) * 10,  
		 fVec3( 0.5f, -0.5f, 0.0f) * 10,  
		 fVec3(-0.5f,  0.5f, 0.0f) * 10, 
		 fVec3( 0.5f,  0.5f, 0.0f) * 10  
	};

	std::array<fVec2, 4> uvs =
	{
		 fVec2(0.f, 0.f), 
		 fVec2(1.f, 0.f), 
		 fVec2(0.f, 1.f), 
		 fVec2(1.f, 1.f)  
	};

	std::array<int, 6> indices = { 0, 2, 1, 2, 3, 1 };

	uint VAO;
	glGenVertexArrays(1, &VAO);
	{
		uint VBO[2], EBO;
		glGenBuffers(2, VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);

		GLHelper::SetVBOData(VBO[0], triangles, 3, 0);
		GLHelper::SetVBOData(VBO[1], uvs, 2, 2);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
	}

	uint ubo = GLHelper::CreateUBO(0, (16 * sizeof(float)) * 3);

	enum NodeType : uchar
	{
		Empty,
		Full,
		Border
	};

	uint VAOMS[4];
	std::vector<fVec3> msEdges, emptyCells, fullCells, btwCells;
	std::array<std::reference_wrapper<std::vector<fVec3>>, 4> containers =
		{ emptyCells, fullCells, btwCells, msEdges };

	glPointSize(5);
	glLineWidth(5);
	glClearColor(0.f, 0.f, 0.5f, 0.f);

	GL::Tex2D cpuTex;
	cpuTex.Filter = GL::TexFilter::Nearest;
	cpuTex.Data = GL::DataType::Float;

	float noiseZ = 0, QEFEpsilon = 0.001f;
	while (!quit)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static bool pauseTime = false;
		if (inputs.IsKeyPressed(Inputs::P)) { pauseTime = !pauseTime; }

		if (inputs.IsKeyPressed(Inputs::T)) { QEFEpsilon *= 10.f; std::cout << "New Epsilon: " << QEFEpsilon << std::endl; }
		if (inputs.IsKeyPressed(Inputs::G)) { QEFEpsilon *= 0.1f; std::cout << "New Epsilon: " << QEFEpsilon << std::endl; }

		if (!pauseTime)
		{
			noiseZ += Time::DeltaTime() * 0.03;
			glDeleteVertexArrays(4, VAOMS);
			auto gridFromTree = GenerateMarchingSquares(VAOMS, noiseZ, QEFEpsilon, msEdges, emptyCells, fullCells, btwCells);
			cpuTex.Release();
			cpuTex.Setup(gridFromTree.Size[0], gridFromTree.Size[1], GL::R32F, &gridFromTree.v[0]);
		}
		cpuTex.Bind();


		dVec2 move;
		move.x = inputs.GetAxis("Horizontal", Inputs::Smooth);
		move.y = inputs.GetAxis("Vertical", Inputs::Smooth);
		float scroll = inputs.GetAxis("Scroll", Inputs::Smooth);
		cam.Update(4.f, true, move, dVec2(), scroll, Time::DeltaTime());

		Math::fMat4 view = Math::GL::LookAt(cam.Pos, cam.Pos - Vector3::Forward, Vector3::Up),
			proj = Math::GL::Perspective(70.f * Math::Deg2Rad, ASPECT, 0.01, 1000.f);
		Math::fMat4 vp = proj * view;

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		GLHelper::BindUBOData(0, 16 * sizeof(float), &view.v[0]);
		GLHelper::BindUBOData(16 * sizeof(float), 16 * sizeof(float), &proj.v[0]);
		GLHelper::BindUBOData(16 * sizeof(float) * 2, 16 * sizeof(float), &vp.v[0]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		texProg->Use();
		texProg->Set("uColor", fVec4(0.2, 0.2, 0.2, 1));
		texProg->Set("uModel", 
			Math::GL::Rotate(Math::GL::Rotate(Math::Mat4(), 
				180.f * Math::Deg2Rad, fVec3::Right), 
				90.f * Math::Deg2Rad, fVec3::Forward));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		solidProg->Use();
		solidProg->Set("uModel", Math::Mat4());
		// Empty | Full | Between | Edge
		std::array<fVec3, 4> colors = 
		{ fVec3(1, 0, 0), fVec3(0, 1, 0), fVec3(0, 0.5, 0.5), fVec3(0, 1, 0) };	
		for (size_t i = 0; i < 4; i++)
		{
			solidProg->Set("uColor", colors[i]);
			glBindVertexArray(VAOMS[i]);
			glDrawArrays(GL_LINES, 0, containers[i].get().size());
		}

		Locator::Call("Update");

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT: quit = true; break;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{ Locator::Call("Inputs/SetKey", event); }
			break;
			case SDL_MOUSEMOTION:
			{ Locator::Call("Inputs/SetMousePos", event); }
			break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{ Locator::Call("Inputs/SetMouseButton", event); }
			break;
			case SDL_MOUSEWHEEL:
			{ Locator::Call("Inputs/SetMouseWheel", event); }
			break;
			}
		}

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}