#define GLEW_STATIC
#define SDL_MAIN_HANDLED
#include "Math/Bezier.h"
#include "Graphics/Camera.h"
#include "Core.h"
#include "Graphics/GL.h"
#include "Graphics/GLTypes.h"
#include "Graphics/GPUGrid.h"
#include "Common/Grid.h"
#include "Graphics/Graphics.h"
#include "Math/MathExt.h"
#include "Graphics/MathGL.h"
#include "Graphics/Mesh.h"
#include "SDL.h"
#include "Graphics/Shader.h"
#include "Common/StringUtils.h"
#include "Common/Systems/Inputs.h"
#include "Common/GameTime.h"
#include "Common/Watch.h"
#include "framework.h"
#include "pch.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Common/FastNoise.h"
#include "GL/glew.h"
#include "stb_image.h"

#include <charconv>
#include <fstream>
#include <iostream>
#include <random>

#define __WINDOWS_WASAPI__
#define __WINDOWS_DS__
#define __WINDOWS_ASIO__

#define WIDTH (1920/2)
#define HEIGHT (1080/2)
#define ASPECT ((float)WIDTH / HEIGHT)

//TODO: ResourceManager: Cache paths (so each resource doesn't have to save all it's dependencies).
//TODO: ResourceManager: Add option to make files temporary or permanent (whether to unload them with 0 refs).
//TODO: Locator: Change name
//TODO: Locator: Make "update" work with optional ticks / time intervals (e.g. for ResourceManager).
//TODO: Shader: Keep values for uniforms when updating shader (in setup()).

int main(int argc, char* argv[])
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

	glCullFace(GL_NONE);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEBUG_OUTPUT);

	// VSync
	SDL_GL_SetSwapInterval(0);
#pragma endregion

	Inputs inputs;
	inputs.AddAxis("Horizontal", Inputs::D, Inputs::A);
	inputs.AddAxis("Vertical", Inputs::W, Inputs::S);
	inputs.AddAxis("Scroll", Inputs::E, Inputs::Q);
	Time time;

	bool quit = false;
	SDL_Event event;

	Time::SetLimitFPS(true);
	Time::SetMaxFPS(60);

	uint ubo = GLHelper::CreateUBO(0, (16 * sizeof(float)) * 3);
	GL::ProgRef blockProg =
		Rscs::Manager::Get().loadResource<GL::Program>({ "Texture.vert", "Tex.frag" });

	iVec2 tileSize(5, 20);

	Camera2D camera(ASPECT);
	camera.Speed = 1.f;
	camera.VertSize = std::max<float>((tileSize.x + 2) / ASPECT, (tileSize.y + 2)) / 2.f;

	while (!quit)
	{
		glClearColor(0.f, 0.26f, 0.2f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		fVec2 movement(inputs.GetAxis("Horizontal", Inputs::Smooth),
			inputs.GetAxis("Vertical", Inputs::Smooth));
		camera.Update(movement, Time::DeltaTime());
		blockProg->Use();
		blockProg->Set("uModel", Math::Mat4());

		//Math::fMat4 view = Math::GL::LookAt(camera.Pos, camera.Pos + fVec3::Forward, Vector3::Up),
		//	proj = Math::GL::Orthographic(-5.f * ASPECT, 5.f * ASPECT, -5.f, 5.f, 0.f, 100.f);
		//glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		//GLHelper::BindUBOData(0, 16 * sizeof(float), &view.v[0]);
		//GLHelper::BindUBOData(16 * sizeof(float), 16 * sizeof(float), &proj.v[0]);
		//GLHelper::BindUBOData(16 * sizeof(float) * 2, 16 * sizeof(float), &(proj * view).v[0]);
		//glBindBuffer(GL_UNIFORM_BUFFER, 0);

		for(float x = -tileSize.x / 2.f; x <= tileSize.x / 2.f; x++)
		{
			for(float y = -tileSize.y / 2.f; y <= tileSize.y / 2.f; y++)
			{
				Graphics::DrawCube(fVec2(x, y), fVec2::One*0.1f, fVec3(1.f, 0.f, 0.f));
			}
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


/*
#define GLEW_STATIC
#define SDL_MAIN_HANDLED
#include "Math/Bezier.h"
#include "Graphics/Camera.h"
#include "Core.h"
#include "Graphics/GL.h"
#include "Graphics/GLTypes.h"
#include "Graphics/GPUGrid.h"
#include "Common/Grid.h"
#include "Math/MathExt.h"
#include "Graphics/MathGL.h"
#include "Graphics/Mesh.h"
#include "SDL.h"
#include "Graphics/Shader.h"
#include "Common/StringUtils.h"
#include "Common/Systems/Inputs.h"
#include "Common/GameTime.h"
#include "Common/Watch.h"
#include "framework.h"
#include "pch.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Common/FastNoise.h"
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

Grid<float, 2, BorderPolicy::Clamp> Caves(1024, 1024);
GL::Mesh CaveMesh;


//TODO: ResourceManager: Cache paths (so each resource doesn't have to save all it's dependencies).
//TODO: ResourceManager: Add option to make files temporary or permanent (whether to unload them with 0 refs).
//TODO: Locator: Change name
//TODO: Locator: Make "update" work with optional ticks / time intervals (e.g. for ResourceManager).
//TODO: Shader: Keep values for uniforms when updating shader (in setup()).

uint CreateMesh(int pyrW, int pyrH, 
	std::shared_ptr<GL::StorageBuffer> &vertBuffer)
{
	uint totalVertCount;
	int texMaxSize = GL::TextureMaxSize();

	//Create the PH texture.
	GL::Tex2D vertCountTex;
	vertCountTex.Setup(std::fminf(pyrW * 2, texMaxSize), std::max((pyrW * 2) / texMaxSize, 1), GL::R32UI);
	glBindImageTexture(1, vertCountTex.ID, 0, GL_FALSE, 0, GL_READ_WRITE, vertCountTex.Format);

	//Generate the base level (based on the configuration of the squares).
	GL::ProgRef baseLvlProg =
		Rscs::Manager::Get().loadResource<GL::Program>({ { "HPBaseLevel.comp" } });
	baseLvlProg->Use();
	baseLvlProg->Dispatch(Caves.Size[0], Caves.Size[1], 1);

	//Bind the buffer to get the total vertex count.
	GL::StorageBuffer vertexCount(0, sizeof(uint), NULL);

	uint lowerOffset = 0, upperOffset = pyrW;
	GL::ProgRef reducProg =
		Rscs::Manager::Get().loadResource<GL::Program>({ { "HPReduction.comp" } });
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
		vertexCount.Unmap();
		Debug::Log("Final Vertex Count: " + std::to_string(int(totalVertCount)));
		std::cout << "TOTALVERTCOUNT FETCH: " << watch.sTime() << std::endl;
	}

	glBindImageTexture(1, vertCountTex.ID, 0, GL_FALSE, 0, GL_READ_WRITE, vertCountTex.Format);

	vertBuffer 
		= std::shared_ptr<GL::StorageBuffer>
			(new GL::StorageBuffer(1, totalVertCount * sizeof(Vector2), NULL));
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	{
		GL::ProgRef expandVert = 
			Rscs::Manager::Get().loadResource<GL::Program>({ { "HPExpansion.comp" } });
		expandVert->Use();
		expandVert->Set("uMaxY", pyrH - 1);
		expandVert->Set("VertCount", totalVertCount);
		expandVert->Set("uTopXOffset", upperOffset);
		expandVert->Dispatch(totalVertCount, 1, 1);
	}

	glDeleteBuffers(1, &vertexCount.id);

	return totalVertCount;
}

int main(int argc, char* argv[])
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

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEBUG_OUTPUT);

	// VSync
	SDL_GL_SetSwapInterval(0);
#pragma endregion

	Inputs inputs;
	inputs.AddAxis("Horizontal", Inputs::E, Inputs::Q);
	inputs.AddAxis("Vertical", Inputs::D, Inputs::A);
	inputs.AddAxis("Scroll", Inputs::S, Inputs::W);
	Time time;

	bool quit = false;
	SDL_Event event;

	Time::SetLimitFPS(true);


	float isoThresh = -0.f;

#pragma region Shader Setup
	GL::ProgRef texShader =
		Rscs::Manager::Get().loadResource<GL::Program>({ { "Texture.vert" }, { "Tex.frag" } });
	GL::ProgRef meshShader =
		Rscs::Manager::Get().loadResource<GL::Program>({ { "Texture.vert" }, { "Tex.frag" } });

	GL::Tex2D loadTex;
	loadTex.Setup("TestTex.png");

#pragma region Compute Cave
	GL::Tex2D noiseTex;
	noiseTex.Data = GL::Float;
	noiseTex.Wrap = GL::Clamp;
	noiseTex.Filter = GL::Nearest;
	noiseTex.Setup(Caves.Size[0], Caves.Size[1], GL::R16F);
	glBindImageTexture(0, noiseTex.ID, 0, GL_FALSE, 0, GL_READ_WRITE, noiseTex.Format);

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
	std::shared_ptr<GL::StorageBuffer> vertBuffer;
	uint totalVertCount = CreateMesh(pyrW, pyrH, vertBuffer);
	caveWatch.Stop();
	std::cout << "Caves: " << caveWatch.sTime() << std::endl;
#pragma endregion

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	std::vector<Vector2> caveMesh;

	caveMesh.resize(totalVertCount);
	void *mesh = vertBuffer->Map();
	memcpy(&caveMesh[0], (Vector2*)mesh, totalVertCount * sizeof(Vector2));
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
		GLHelper::SetVBOData(caveMesh, 2, 1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	uint ubo = GLHelper::CreateUBO(0, (16 * sizeof(float)) * 3);
#pragma endregion

	glLineWidth(3.f);

	Camera cam(Vector3(0.f, 0.f, 10.f), Vector3::Forward);

	auto file = Rscs::Manager::Get().loadResource<GL::Program>({ "HPBaseLevel.comp" });

	//auto file = Resource1::Manager::Get().Load<StringWrapper>("HPBaseLevel.comp");
	////file = ResourceManager::inst().Load<StringWrapper>("HPBaseLevel.comp");
	//file->Test();
	//file->dependencies.push_back(Resource1::FileInfo());
	//auto &rsc = Resource1::Manager::Get();

	auto &cx = Rscs::Manager::Get();
	meshShader->Use();		
	meshShader->Set("uColor", Vector3(1.f));
	meshShader->Set("uAlpha", 1.f);
 	while (!quit)
	{
		glClearColor(0.f, 0.f, 0.5f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		dVec2 move;
		move.x = inputs.GetAxis("Horizontal");
		move.y = inputs.GetAxis("Vertical");
		float scroll = inputs.GetAxis("Scroll");
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


		glActiveTexture(GL::Texture0);
		loadTex.Bind();
		meshShader->Use();
		Math::Mat4 mat;
		//mat = Math::GL::Translate(mat, Vector3(tf.Pos.x / ASPECT, tf.Pos.y, 0.f));
		mat = Math::GL::Scale(mat, Vector3(0.1f));
		//mat = Math::GL::Rotate(mat, Math::Deg2Rad * tf.Rot, Vector3::Forward);
		//mat = Math::GL::Scale(mat, /*Vector3(tex->Ratio(), 1.f) * / Vector3(tf.Scale) / (8.f * ASPECT));
		meshShader->Set("uModel", mat);

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
*/