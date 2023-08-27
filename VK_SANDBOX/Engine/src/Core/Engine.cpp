#include "../pch.h"
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

	IHCEngine::Core::Time::Init();
	Time::GetInstance().LockFrameRate(120);
	Time::GetInstance().SetFixedTime(Time::FIXED_UPDATE_TIME_STEP);

	graphicsManager = std::make_unique<Graphics::GraphicsManager>(appWindow);
	graphicsManager->Init();
	//application->Init();
}

void IHCEngine::Core::Engine::Update()
{
	while (!appWindow->ShouldClose())
	{
		Time::GetInstance().Update();

		//application->Update();

		// Time::Reset(); // if scene change

		while (Time::GetInstance().ShouldExecuteFixedUpdate())
		{
			Time::GetInstance().UpdateFixedTime();
			//component->FixedUpdate();
			//physics->Update();

		}

		graphicsManager->Update();
	}
}

void IHCEngine::Core::Engine::Shutdown()
{
	//application->Shutdown();
	graphicsManager->Shutdown();
}
