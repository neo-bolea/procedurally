#pragma region Includes
#define GLEW_STATIC
#define SDL_MAIN_HANDLED
#include "Bezier.h"
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
	fn.GradientPerturb(coords.x, coords.y, coords.z);
	return fn.GetNoise(coords.x, coords.y, coords.z);
}

fVec2 fD(FastNoise fn, fVec3 coords)
{
	float epsilon = 0.001f;
	fVec2 d;
	d.x = f(fn, fVec3(coords.x + epsilon, coords.y, coords.z)) - f(fn, fVec3(coords.x - epsilon, coords.y, coords.z));
	d.y = f(fn, fVec3(coords.x, coords.y + epsilon, coords.z)) - f(fn, fVec3(coords.x, coords.y - epsilon, coords.z));
	return d.Normalize();
}


auto GenerateMarchingSquares(
	uint VAO[4], float noiseZ, 
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
		bool operator !=(const BorderInfo &other) { return Config != other.Config; }

		BorderInfo operator +(const BorderInfo &other) { return BorderInfo((Value + other.Value) / 2., Config); }
		BorderInfo operator /(uint other) { return BorderInfo(Value / other, Config); }
	};

#ifdef NDEBUG
	Grid<float> gridTest(256, 256);
	LinearTree<BorderInfo, 8, CoordinateSystem::Normalized> tree(BorderInfo(0, 0));
#else
	Grid<float> gridTest(64, 64);
	LinearTree<BorderInfo, 6, CoordinateSystem::Normalized> tree(BorderInfo(0, 0));
#endif

	Watch watchTotal(Watch::ms);
	watchTotal.Start();
	//tree.Map([&gridTest](decltype(tree)::NodeInfo& info) { info.Node = HIShape(Point(info.Pos[0], info.Pos[1])) < 0; });
	FastNoise fn;
	fn.SetFractalType(FastNoise::FBM);
	fn.SetCellularReturnType(FastNoise::Distance);
	fn.SetCellularDistanceFunction(FastNoise::Euclidean);

	fn.SetFrequency(4);
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

	tree.Map(
		[&](decltype(tree)::NodeInfo info)
		{
			//if (info.Node.Data != 2) { return; }

			fVec2 pos = fVec2(info.Pos) + fVec2(info.Size / 2);
			float size = info.Size;
			fVec2
				BotLefCor = pos + fVec2(-size, -size) / 2,
				BotRigCor = pos + fVec2(size, -size) / 2,
				TopRigCor = pos + fVec2(size, size) / 2,
				TopLefCor = pos + fVec2(-size, size) / 2;

			std::array<float, 4> values =
			{
				info.Node.Data.Value,
				f(fn, fVec3(BotRigCor.x, BotRigCor.y, noiseZ)/* + fVec3(0.0001f, 0.0001f, 0)*/),
				f(fn, fVec3(TopRigCor.x, TopRigCor.y, noiseZ)/* + fVec3(0.0001f, 0.0001f, 0)*/),
				f(fn, fVec3(TopLefCor.x, TopLefCor.y, noiseZ)/* + fVec3(0.0001f, 0.0001f, 0)*/),
			};

 			int config = (values[0] > 0) + ((values[1] > 0) + ((values[2] > 0) + ((values[3] > 0) << 1) << 1) << 1);
			//if (config == 15 || config == 0) { return; }

			fVec2
				BotCenEdge = fVec2(BotRigCor.x + -((0 - values[1]) / (values[0] - values[1])) * size, BotLefCor.y),
				CenRigEdge = fVec2(BotRigCor.x, TopRigCor.y + -((0 - values[2]) / (values[1] - values[2])) * size),
				TopCenEdge = fVec2(TopRigCor.x + -((0 - values[2]) / (values[3] - values[2])) * size, TopRigCor.y),
				CenLefEdge = fVec2(TopLefCor.x, TopLefCor.y + -((0 - values[3]) / (values[0] - values[3])) * size);


			if(info.Node.Data.Config == 2)
			{
				std::array<fVec2, 4> intersections =
				{
					BotCenEdge,
					CenRigEdge,
					TopCenEdge,
					CenLefEdge,
				};

				std::array<fVec2, 4> normals =
				{
					fD(fn, (fVec3)BotCenEdge),
					fD(fn, (fVec3)CenRigEdge),
					fD(fn, (fVec3)TopCenEdge),
					fD(fn, (fVec3)CenLefEdge),
				};

				Triangulate(btwCells, pos, pos + fVec2(0.0001f, 0.f));
			}

			//if (info.Node.Data.Config == 1)
			//{
			//	Triangulate(fullCells, BotLefCor, BotRigCor);
			//	Triangulate(fullCells, BotRigCor, TopRigCor);
			//	Triangulate(fullCells, TopRigCor, TopLefCor);
			//	Triangulate(fullCells, TopLefCor, BotLefCor);
			//}
			//else if(info.Node.Data.Config == 2)
			//{
			//	Triangulate(btwCells, BotLefCor, BotRigCor);
			//	Triangulate(btwCells, BotRigCor, TopRigCor);
			//	Triangulate(btwCells, TopRigCor, TopLefCor);
			//	Triangulate(btwCells, TopLefCor, BotLefCor);
			//}
			//else
			//{
			//	Triangulate(emptyCells, BotLefCor, BotRigCor);
			//	Triangulate(emptyCells, BotRigCor, TopRigCor);
			//	Triangulate(emptyCells, TopRigCor, TopLefCor);
			//	Triangulate(emptyCells, TopLefCor, BotLefCor);
			//}

			switch (config)
			{
			case 1:  Triangulate(msEdges, CenLefEdge, BotCenEdge); return;
			case 2:  Triangulate(msEdges, BotCenEdge, CenRigEdge); return;
			case 4:  Triangulate(msEdges, CenRigEdge, TopCenEdge); return;
			case 8:  Triangulate(msEdges, TopCenEdge, CenLefEdge); return;

			//4 Vertices      
			case 3:  Triangulate(msEdges, CenLefEdge, CenRigEdge); return;
			case 6:  Triangulate(msEdges, BotCenEdge, TopCenEdge); return;
			case 9:  Triangulate(msEdges, BotCenEdge, TopCenEdge); return;
			case 12: Triangulate(msEdges, CenRigEdge, CenLefEdge); return;

			//5 Vertices   
			case 7:  Triangulate(msEdges, CenLefEdge, TopCenEdge); return;
			case 11: Triangulate(msEdges, TopCenEdge, CenRigEdge); return;
			case 13: Triangulate(msEdges, CenRigEdge, BotCenEdge); return;
			case 14: Triangulate(msEdges, BotCenEdge, CenLefEdge); return;

			//6 Vertices      
			case 5:  Triangulate(msEdges, CenLefEdge, TopCenEdge); Triangulate(msEdges, CenRigEdge, BotCenEdge); return;
			case 10: Triangulate(msEdges, TopCenEdge, CenRigEdge); Triangulate(msEdges, BotCenEdge, CenLefEdge); return;
			}
		});

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

	float noiseZ = 0;
	while (!quit)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static bool pauseTime = false;
		if (inputs.IsKeyPressed(Inputs::P)) { pauseTime = !pauseTime; }

		noiseZ += Time::DeltaTime() * 0.03 * !pauseTime;
		glDeleteVertexArrays(4, VAOMS);
		auto gridFromTree = GenerateMarchingSquares(VAOMS, noiseZ, msEdges, emptyCells, fullCells, btwCells);
		cpuTex.Release();
		cpuTex.Setup(gridFromTree.Size[0], gridFromTree.Size[1], GL::R32F, &gridFromTree.v[0]);
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