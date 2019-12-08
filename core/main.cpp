#define GLEW_STATIC
#define SDL_MAIN_HANDLED
#include "Bezier.h"
#include "Camera.h"
#include "Core.h"
#include "GL.h"
#include "Grid.h"
#include "MathExt.h"
#include "MathGL.h"
#include "Mesh.h"
#include "SDL.h"
#include "Shader.h"
#include "StringUtils.h"
#include "Systems/Inputs.h"
#include "Time.h"
#include "Watch.h"
#include "framework.h"
#include "pch.h"

#define STB_IMAGE_IMPLEMENTATION
#include "GL/glew.h"
#include "stb_image.h"

#include <charconv>
#include <fstream>
#include <iostream>
#include <random>

#define __WINDOWS_WASAPI__
#define __WINDOWS_DS__
#define __WINDOWS_ASIO__

#define WIDTH (1920 / 2)
#define HEIGHT (1080 / 2)

Grid<float, 2, BorderPolicy::Clamp> Caves(0, 0);
GL::Mesh CaveMesh;
uint totalVertCount;

struct NoisePass
{
	FastNoise::NoiseType Type = FastNoise::Perlin;
	FastNoise::Interp Interp = FastNoise::Linear;
	FastNoise::FractalType FracType = FastNoise::Billow;
	FastNoise::CellularDistanceFunction DistType = FastNoise::Euclidean;
	FastNoise::CellularReturnType CellType = FastNoise::Distance;
	float Strength = 1.f;
	float Freq = 0.01f;
	float PertAmp = 1.f;

	int Octaves = 1;
	float Lacunarity = 2.f;
	float Gain = 0.5f;
};

void GenerateCave(std::vector<NoisePass>& passes)
{
	Caves.ForAll([](int x, int y) { Caves(x, y) = 0.f; });

	FastNoise fn(1337);
	Debug::Watch watch("Generate Noise");
	for (size_t i = 0; i < passes.size(); i++)
	{
		auto pass = passes[i];

		fn.SetNoiseType(pass.Type);
		fn.SetInterp(pass.Interp);
		fn.SetFractalType(pass.FracType);
		fn.SetCellularDistanceFunction(pass.DistType);
		fn.SetCellularReturnType(pass.CellType);
		fn.SetFrequency(pass.Freq);
		fn.SetGradientPerturbAmp(pass.PertAmp);

		fn.SetFractalOctaves(pass.Octaves);
		fn.SetFractalLacunarity(pass.Lacunarity);
		fn.SetFractalGain(pass.Gain);

		for (float x = 0; x < Caves.Length[0]; x++)
		{
			for (float y = 0; y < Caves.Length[1]; y++)
			{
				float xx = x, yy = y;
				fn.GradientPerturb(xx, yy);
				float noise = fn.GetNoise(xx, yy);
				Caves(x, y) += noise * pass.Strength;
			}
		}
	}
	watch.StopTime();
}

uint totalVertCount;
std::unique_ptr<StorageBuffer> CreateMesh(int pyrW, int pyrH)
{
	int texMaxSize = GL::TextureMaxSize();
	IntVector3 workSize;

	//Create the PH texture.
	GL::Tex2D vertCountTex;
	vertCountTex.Setup(std::fminf(pyrW * 2, texMaxSize), std::fmaxf((pyrW * 2) / texMaxSize, 1), GL::R32UI);
	glBindImageTexture(1, vertCountTex.ID, 0, GL_FALSE, 0, GL_READ_WRITE, vertCountTex.Format);

	//Generate the base level (based on the configuration of the squares).
	GL::ProgRef baseLvlProg = GL::Programs.Load({ { "HPBaseLevel.comp" } });
	baseLvlProg->Use();
	glGetProgramiv(baseLvlProg->id, GL_COMPUTE_WORK_GROUP_SIZE, &workSize.e[0]);
	int dispatchX = Math::NextPowerOfTwo(Caves.Length[0] / workSize.x),
		dispatchY = Math::NextPowerOfTwo(Caves.Length[1] / workSize.y);
	glDispatchCompute(std::fmaxf(dispatchX, 1), std::fmaxf(dispatchY, 1), 1);

	//Bind the buffer to get the total vertex count.
	StorageBuffer vertexCount(0, sizeof(uint), NULL);

	//Reduce the PH levels until we reach the top level (which has a single node with the total vertex count).
	GL::ProgRef reducProg = GL::Programs.Load({ { "HPReduction.comp" } });
	glGetProgramiv(reducProg->id, GL_COMPUTE_WORK_GROUP_SIZE, &workSize.e[0]);
	reducProg->Use();

	uint uCellCount = Math::NextPowerOfTwo(pyrW) / 2;
	uint lowerOffset = 0, upperOffset = pyrW;

	glGetProgramiv(reducProg->id, GL_COMPUTE_WORK_GROUP_SIZE, &workSize.e[0]);
	for (uint i = 0; i < pyrH; i++)
	{
		//TODO: Get WorkSize
		reducProg->Set("uCellCount", uCellCount);
		reducProg->Set("uLowerXOffset", lowerOffset);
		reducProg->Set("uUpperXOffset", upperOffset);
		glDispatchCompute(std::fmaxf(uCellCount / workSize.x, 1), 1, 1);
		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

		Debug::Log("PASS: " + std::to_string(upperOffset) + ", " + std::to_string(uCellCount));

		lowerOffset = upperOffset;
		upperOffset += uCellCount;
		uCellCount /= 2;
	}

	void* bufferPointer = vertexCount.Map();
	memcpy(&totalVertCount, bufferPointer, sizeof(uint));
	Debug::Log("Final Vertex Count: " + std::to_string(int(totalVertCount)));
	vertexCount.Unmap();

	std::unique_ptr<StorageBuffer> meshBuffer(new StorageBuffer(1, totalVertCount * sizeof(Vector2), NULL));
	//meshBuffer->SetData(&totalVertCount, 0, sizeof(uint));

	//Create the mesh texture
	//GL::Tex2D caveTex;
	//int wrappedHeight = totalVertCount / texMaxSize + 1;
	//caveTex.Setup(std::fminf(totalVertCount, texMaxSize), wrappedHeight, GL::RG32F);
	//glBindImageTexture(2, caveTex.ID, 0, GL_FALSE, 0, GL_WRITE_ONLY, caveTex.Format);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	GL::ProgRef expandProg = GL::Programs.Load({ { "HPExpansion.comp" } });
	expandProg->Use();
	expandProg->Set("uMaxY", pyrH - 1);
	expandProg->Set("VertCount", totalVertCount);
	expandProg->Set("uTopXOffset", upperOffset);
	Debug::Log("TOP" + std::to_string(upperOffset));
	glGetProgramiv(expandProg->id, GL_COMPUTE_WORK_GROUP_SIZE, &workSize.e[0]);
	int maxGroup;
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxGroup);
	glDispatchCompute(Math::NextPowerOfTwo(totalVertCount / workSize.x), 1, 1);

	vertCountTex.Release();
	glDeleteBuffers(1, &vertexCount.id);

	return std::move(meshBuffer);
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
	glEnable(GL_CULL_FACE);

	// Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Depth Test
	glEnable(GL_DEPTH_TEST);

	// Multi Sampling
	glEnable(GL_MULTISAMPLE);

	// Errors
	glEnable(GL_DEBUG_OUTPUT);

	// VSync
	SDL_GL_SetSwapInterval(0);
#pragma endregion

	Inputs inputs;
	Time time;

	bool quit = false;
	SDL_Event event;

	Time::SetLimitFPS(true);

	std::vector<fVec2> ctrlPoints =
	{
		fVec2(-0.2f, -0.2f),
		fVec2(-0.2f, 0.2f),
		fVec2(0.2f, 0.2f),
		fVec2(0.2f, -0.2f),
	};

	GL::ProgRef prog = GL::Programs.Load({ "Line.vert", "Line.frag" });
	std::vector<fVec2> vertices;

	Watch watch(Watch::ms);
	watch.Start();
	Bezier::GenerateSpline(ctrlPoints, vertices, 1000);
	watch.Stop();
	std::cout << "TIME: " << watch.sTime();

	uint LinesVAO;
	{
		uint VBO;
		glGenVertexArrays(1, &LinesVAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(LinesVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		GLHelper::SetVBOData(vertices, 2, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	uint PointsVAO;
	{
		uint VBO;
		glGenVertexArrays(1, &PointsVAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(PointsVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		GLHelper::SetVBOData(ctrlPoints, 2, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glPointSize(4);
	glLineWidth(4);

//	float isoThresh = -0.28f;
//	Caves.Length[0] = 512;
//	Caves.Length[1] = 512;
//
//#pragma region Noise
//	NoisePass np;
//	np.Type = FastNoise::SimplexFractal;
//	np.FracType = FastNoise::Billow;
//	np.Freq = 0.0024f;
//	np.Octaves = 4;
//	np.Lacunarity = 2.f;
//	np.Gain = 0.5f;
//
//	np.Type = FastNoise::Perlin;
//	np.FracType = FastNoise::FBM;
//	np.Freq = 0.0024f;
//	np.Octaves = 1;
//	np.Lacunarity = 2.f;
//	np.Gain = 0.5f;
//
//	std::vector<NoisePass> npList = { np };
//#pragma endregion
//
//#pragma region Shader Setup
//	GL::ProgRef texShader = GL::Programs.Load({ { "Texture.vert" }, { "Tex.frag"} });
//	GL::ProgRef meshShader = GL::Programs.LoadDefaultProgram(GL::ProgramPool::eSolid3D);
//	meshShader->Use();
//	auto ident = Math::GL::Identity(4);
//	meshShader->Set("uColor", Vector3(1.f));
//
//#pragma region Compute Cave
//	GL::Tex2D noiseTex;
//	noiseTex.Data = GL::Float;
//	noiseTex.Wrap = GL::Clamp;
//	noiseTex.Filter = GL::Nearest;
//	noiseTex.Setup(Caves.Length[0], Caves.Length[1], GL::R16F);
//	glBindImageTexture(0, noiseTex.ID, 0, GL_FALSE, 0, GL_READ_WRITE, noiseTex.Format);
//
//	SpriteRef caveSpr = rend.CreateSprite();
//	caveSpr->SetTex(std::make_shared<GL::Tex2D>(noiseTex));
//
//	Compute::Noise cn;
//	cn.Octaves.Set(4);
//	cn.Frequency.Set(5.f);
//	cn.Gain.Set(0.5f);
//	cn.Lacunarity.Set(2.f);
//
//	Debug::Watch watch("Noise");
//	cn.GetNoise(std::make_shared<GL::Tex2D>(noiseTex));
//	cn.WaitFinish();
//
//	int pyrW = Math::NextPowerOfTwo(Caves.Length[0] * Caves.Length[1]);
//	int pyrH = (int)log2(pyrW);
//	Debug::Log(IntVector2(pyrW, pyrH).ToString());
//	auto meshBuffer = CreateMesh(pyrW, pyrH);
//	watch.StopTime();
//#pragma endregion
//
//
//	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//	std::vector<Vector2> caveMesh;
//	caveMesh.resize(totalVertCount);
//	//void *mesh = meshBuffer->Map();
//	//memcpy(&caveMesh[0], (Vector2 *)mesh, totalVertCount * sizeof(Vector2));
//	//meshBuffer->Unmap();
//
//	uint VAO, VBO;
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//	glBindVertexArray(VAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer->id);
//	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Vector2), 0);
//	glEnableVertexAttribArray(0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	uint ubo = GLHelper::CreateUBO(0, (16 * sizeof(float)) * 3);
#pragma endregion

	Grid<float> mat2(10000, 10000);
	for (size_t i = 0; i < mat2.Size[0]; i++)
	{
		for (size_t j = 0; j < mat2.Size[1]; j++)
		{
			mat2[i][j] = i * (mat2.Size[0] - 1) + j;
		}
	}

	int xi = 0;
	Watch watch1(Watch::ms);
	watch1.Start();
	mat2.ForAll([&xi, &mat2](const std::array<size_t, 2>& arr)
		{
			xi += mat2[arr[0]][arr[1]];
		});
	watch1.Stop();
	std::cout << xi << std::endl;
	std::cout << watch1.sTime() << std::endl;


	while (!quit)
	{
		glClearColor(0.f, 0.f, 0.5f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		prog->Use();
		prog->Set("uColor", fVec3(1, 0, 0));
		glBindVertexArray(LinesVAO);
		glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
		prog->Set("uColor", fVec3(0, 1, 0));
		glBindVertexArray(PointsVAO);
		glDrawArrays(GL_POINTS, 0, ctrlPoints.size());
		glBindVertexArray(0);

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