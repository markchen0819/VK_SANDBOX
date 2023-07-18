#include "Engine.h"

//STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
// tiny loader
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>


IHCEngine::Core::Engine::Engine()
{

}

void IHCEngine::Core::Engine::Init()
{
	appWindow = std::make_shared<Window::AppWindow>("MARKENGINE", 800, 600);
	renderSystem = std::make_shared<Graphics::RenderSystem>();


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
	appWindow->Shutdown();
}
