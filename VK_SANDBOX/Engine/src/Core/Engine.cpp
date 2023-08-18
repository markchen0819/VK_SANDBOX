#include "Engine.h"

//STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
// tiny loader
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>


IHCEngine::Core::Engine::Engine()
{
	// explicitly call Init & Shutdown
}

void IHCEngine::Core::Engine::Init()
{
	appWindow = std::make_unique<Window::AppWindow>("MARKAPP", 800, 600);
	renderSystem = std::make_unique<Graphics::RenderSystem>();
	renderSystem->Init(appWindow);
	//application->Init();
}

void IHCEngine::Core::Engine::Update()
{
	while (!appWindow->ShouldClose())
	{
		//application->Update();
		renderSystem->Update();
	}
}

void IHCEngine::Core::Engine::Shutdown()
{
	//application->Shutdown();
	renderSystem->Shutdown();
}
