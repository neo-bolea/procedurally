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

#define __WINDOWS_WASAPI__
#define __WINDOWS_DS__
#define __WINDOWS_ASIO__

#define WIDTH (1920)
#define HEIGHT (1080)
#define ASPECT ((float)WIDTH / HEIGHT)

Grid<float, 2, BorderPolicy::Clamp> Caves(16, 16);
GL::Mesh CaveMesh;
uint totalVertCount, totalEdgeCount;

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
	Caves.ForAll([](const std::array<size_t, 2> &coords) { Caves(coords) = 0.f; });

	FastNoise fn(1337);
	Watch watch(Watch::ms);
	watch.Start();
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

		for (float x = 0; x < Caves.Size[0]; x++)
		{
			for (float y = 0; y < Caves.Size[1]; y++)
			{
				float xx = x, yy = y;
				fn.GradientPerturb(xx, yy);
				float noise = fn.GetNoise(xx, yy);
				Caves(x, y) += noise * pass.Strength;
			}
		}
	}
	watch.Stop();
	std::cout << "Generate Noise: " << watch.sTime() << std::endl;
}

void CreateMesh(int pyrW, int pyrH, std::shared_ptr<GL::StorageBuffer> &vertBuffer, std::shared_ptr<GL::StorageBuffer> &edgeBuffer)
{
	int texMaxSize = GL::TextureMaxSize();

	//Create the PH texture.
	GL::Tex2D vertCountTex, edgeCountTex;
	vertCountTex.Setup(std::fminf(pyrW * 2, texMaxSize), std::fmaxf((pyrW * 2) / texMaxSize, 1), GL::R32UI);
	glBindImageTexture(1, vertCountTex.ID, 0, GL_FALSE, 0, GL_READ_WRITE, vertCountTex.Format);

	edgeCountTex.Setup(std::fminf(pyrW * 2, texMaxSize), std::fmaxf((pyrW * 2) / texMaxSize, 1), GL::R32UI);
	glBindImageTexture(2, edgeCountTex.ID, 0, GL_FALSE, 0, GL_READ_WRITE, edgeCountTex.Format);

	//Generate the base level (based on the configuration of the squares).
	GL::ProgRef baseLvlProg = GL::Programs.Load({ { "HPBaseLevel.comp" } });
	baseLvlProg->Use();
	baseLvlProg->Dispatch(Caves.Size[0], Caves.Size[1], 1);

	//Bind the buffer to get the total vertex count.
	GL::StorageBuffer vertexCount(0, sizeof(uint), NULL);

	uint lowerOffset = 0, upperOffset = pyrW;
	GL::ProgRef reducProg = GL::Programs.Load({ { "HPReduction.comp" } });
	//Reduce the PH levels until we reach the top level (which has a single node with the total vertex count).
	{
		glBindImageTexture(1, vertCountTex.ID, 0, GL_FALSE, 0, GL_READ_WRITE, vertCountTex.Format);
		reducProg->Use();

		uint uCellCount = Math::NextPowerOfTwo(pyrW) / 2;
		lowerOffset = 0, upperOffset = pyrW;

		for (uint i = 0; i < pyrH; i++)
		{
			reducProg->Set("uCellCount", uCellCount);
			reducProg->Set("uLowerXOffset", lowerOffset);
			reducProg->Set("uUpperXOffset", upperOffset);
			reducProg->Dispatch(uCellCount, 1, 1);
			glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

			lowerOffset = upperOffset;
			upperOffset += uCellCount;
			uCellCount /= 2;
		}

		Watch watch(Watch::ms);
		watch.Start();

		void* bufferPointer = vertexCount.Map();
		watch.Stop();
		memcpy(&totalVertCount, bufferPointer, sizeof(uint));
		Debug::Log("Final Vertex Count: " + std::to_string(int(totalVertCount)));
		vertexCount.Unmap();
		std::cout << "TOTALVERTCOUNT FETCH: " << watch.sTime() << std::endl;
	}

	GL::StorageBuffer edgeCount(0, sizeof(uint), NULL);
	//Reduce the PH levels until we reach the top level (which has a single node with the total vertex count).
	{
		glBindImageTexture(1, edgeCountTex.ID, 0, GL_FALSE, 0, GL_READ_WRITE, edgeCountTex.Format);
		reducProg->Use();

		uint uCellCount = Math::NextPowerOfTwo(pyrW) / 2;
		lowerOffset = 0, upperOffset = pyrW;

		for (uint i = 0; i < pyrH; i++)
		{
			reducProg->Set("uCellCount", uCellCount);
			reducProg->Set("uLowerXOffset", lowerOffset);
			reducProg->Set("uUpperXOffset", upperOffset);
			reducProg->Dispatch(uCellCount, 1, 1);
			glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

			lowerOffset = upperOffset;
			upperOffset += uCellCount;
			uCellCount /= 2;
		}

		Watch watch(Watch::ms);
		watch.Start();

		void* bufferPointer = edgeCount.Map();
		watch.Stop();
		memcpy(&totalEdgeCount, bufferPointer, sizeof(uint));
		Debug::Log("Final Edge Count: " + std::to_string(int(totalEdgeCount)));
		vertexCount.Unmap();
		std::cout << "TOTALEDGECOUNT FETCH: " << watch.sTime() << std::endl;
	}
	glBindImageTexture(1, vertCountTex.ID, 0, GL_FALSE, 0, GL_READ_WRITE, vertCountTex.Format);

	//meshBuffer->SetData(&totalVertCount, 0, sizeof(uint));

	//Create the mesh texture
	//GL::Tex2D caveTex;
	//int wrappedHeight = totalVertCount / texMaxSize + 1;
	//caveTex.Setup(std::fminf(totalVertCount, texMaxSize), wrappedHeight, GL::RG32F);
	//glBindImageTexture(2, caveTex.ID, 0, GL_FALSE, 0, GL_WRITE_ONLY, caveTex.Format);

	vertBuffer = std::shared_ptr<GL::StorageBuffer>(new GL::StorageBuffer(1, totalVertCount * sizeof(Vector2), NULL));
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	{
		GL::ProgRef expandVertProg = GL::Programs.Load({ { "HPExpansionVert.comp" } });
		expandVertProg->Use();
		expandVertProg->Set("uMaxY", pyrH - 1);
		expandVertProg->Set("VertCount", totalVertCount);
		expandVertProg->Set("uTopXOffset", upperOffset);
		expandVertProg->Dispatch(totalVertCount, 1, 1);
	}

	glBindImageTexture(1, edgeCountTex.ID, 0, GL_FALSE, 0, GL_READ_WRITE, edgeCountTex.Format);
	edgeBuffer = std::shared_ptr<GL::StorageBuffer>(new GL::StorageBuffer(1, totalEdgeCount * sizeof(Vector2), NULL));
	{
		GL::ProgRef expandEdgeProg = GL::Programs.Load({ { "HPExpansionEdge.comp" } });
		expandEdgeProg->Use();
		expandEdgeProg->Set("uMaxY", pyrH - 1);
		expandEdgeProg->Set("VertCount", totalEdgeCount);
		expandEdgeProg->Set("uTopXOffset", upperOffset);
		expandEdgeProg->Dispatch(totalEdgeCount, 1, 1);
	}
	 
	glDeleteBuffers(1, &vertexCount.id);
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


	float isoThresh = -0.f;

#pragma region Noise
	NoisePass np;
	np.Type = FastNoise::SimplexFractal;
	np.FracType = FastNoise::Billow;
	np.Freq = 0.0024f;
	np.Octaves = 4;
	np.Lacunarity = 2.f;
	np.Gain = 0.5f;

	np.Type = FastNoise::Perlin;
	np.FracType = FastNoise::FBM;
	np.Freq = 0.0024f;
	np.Octaves = 1;
	np.Lacunarity = 2.f;
	np.Gain = 0.5f;

	std::vector<NoisePass> npList = { np };
#pragma endregion

#pragma region Shader Setup
	GL::ProgRef texShader = GL::Programs.Load({ { "Texture.vert" }, { "Tex.frag"} });
	GL::ProgRef meshShader = GL::Programs.Load({ { "Min3D.vert" }, { "Solid.frag"} });
	meshShader->Use();
	Math::Mat4 ident;
	meshShader->Set("uColor", Vector3(1.f));

#pragma region Compute Cave
	GL::Tex2D noiseTex;
	noiseTex.Data = GL::Float;
	noiseTex.Wrap = GL::Clamp;
	noiseTex.Filter = GL::Nearest;
	noiseTex.Setup(Caves.Size[0], Caves.Size[1], GL::R16F);
	glBindImageTexture(0, noiseTex.ID, 0, GL_FALSE, 0, GL_READ_WRITE, noiseTex.Format);

	//SpriteRef caveSpr = rend.CreateSprite();
	//caveSpr->SetTex(std::make_shared<GL::Tex2D>(noiseTex));

	Compute::Noise cn;
	cn.SetOctaves(4);
	cn.SetFrequency(5.f);
	cn.SetGain(0.5f);
	cn.SetLacunarity(2.f);

	Watch caveWatch(Watch::ms);
	caveWatch.Start();
	cn.GetNoise(noiseTex);
	cn.WaitFinish();

	int pyrW = Math::NextPowerOfTwo(Caves.Size[0] * Caves.Size[1]);
	int pyrH = (int)log2(pyrW);
	//Debug::Log(IntVector2(pyrW, pyrH).ToString());
	std::shared_ptr<GL::StorageBuffer> vertBuffer, edgeBuffer;
	CreateMesh(pyrW, pyrH, vertBuffer, edgeBuffer);
	caveWatch.Stop();
	std::cout << "Caves: " << caveWatch.sTime() << std::endl;
#pragma endregion


	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	std::vector<Vector2> caveMesh, linesMesh;


	linesMesh.resize(totalEdgeCount);
	void* lines = edgeBuffer->Map();
	memcpy(&linesMesh[0], (Vector2*)lines, totalEdgeCount * sizeof(Vector2));
	edgeBuffer->Unmap();

	caveMesh.resize(totalVertCount);
	void *mesh = vertBuffer->Map();
	memcpy(&caveMesh[0], (Vector2 *)mesh, totalVertCount * sizeof(Vector2));
	vertBuffer->Unmap();


	uint VAO, MeshLinesVAO;
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &MeshLinesVAO);
	{
		uint VBO;
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, vertBuffer->id);
		GLHelper::SetVBOData(caveMesh, 2, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	{
		uint VBO;
		glGenBuffers(1, &VBO);
		glBindVertexArray(MeshLinesVAO);

		glBindBuffer(GL_ARRAY_BUFFER, edgeBuffer->id);
		GLHelper::SetVBOData(linesMesh, 2, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	uint ubo = GLHelper::CreateUBO(0, (16 * sizeof(float)) * 3);
#pragma endregion

	glLineWidth(3.f);

	Camera cam(Vector3(0.f, 0.f, 10.f), Vector3::Forward);

	while (!quit)
	{
		glClearColor(0.f, 0.f, 0.5f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		dVec2 move;
		move.x = inputs.IsKeyDown(SDL_SCANCODE_E) - inputs.IsKeyDown(SDL_SCANCODE_Q);
		move.y = inputs.IsKeyDown(SDL_SCANCODE_D) - inputs.IsKeyDown(SDL_SCANCODE_A);
		float scroll = inputs.IsKeyDown(SDL_SCANCODE_S) - inputs.IsKeyDown(SDL_SCANCODE_W);
		cam.Update(10.f, true, move, dVec2(), scroll, Time::DeltaTime());
		Math::fMat4 view = Math::GL::LookAt(cam.Pos, cam.Pos - Vector3::Forward, Vector3::Up), 
			proj = Math::GL::Perspective(Math::Deg2Rad * 70.f, ASPECT, 0.01f, 1000.f);
		Math::fMat4 vp = proj * view;
		//caveShader->Use();
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		GLHelper::BindUBOData(0, 16 * sizeof(float), &view.v[0]);
		GLHelper::BindUBOData(16 * sizeof(float), 16 * sizeof(float), &proj.v[0]);
		GLHelper::BindUBOData(16 * sizeof(float) * 2, 16 * sizeof(float), &vp.v[0]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);


		meshShader->Use();
		Math::Mat4 mat;
		//mat = Math::GL::Translate(mat, Vector3(tf.Pos.x / ASPECT, tf.Pos.y, 0.f));
		mat = Math::GL::Scale(mat, Vector3(0.1f));
		//mat = Math::GL::Rotate(mat, Math::Deg2Rad * tf.Rot, Vector3::Forward);
		//mat = Math::GL::Scale(mat, /*Vector3(tex->Ratio(), 1.f) */ Vector3(tf.Scale) / (8.f * ASPECT));
		meshShader->Set("uModel", mat);

		meshShader->Set("uColor", fVec4(1.f, 0.f, 0.f, 1.f));
		glBindVertexArray(MeshLinesVAO);
		glDrawArrays(GL_LINE_STRIP, 0, totalEdgeCount);

		meshShader->Set("uColor", fVec4(1.f));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, totalVertCount);

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