#include "../pch.h"
#include "Engine.h"
#include "Locator/Locators.h"

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
	assert(application != nullptr, "Engine application can't be null");

	// Window
	appWindow = std::make_unique<Window::AppWindow>(application->GetName(), 800, 600);
	// Time
	IHCEngine::Core::Time::Init();
	Time::GetInstance().LockFrameRate(120);
	Time::GetInstance().SetFixedTime(Time::FIXED_UPDATE_TIME_STEP);
	// Asset
	assetManager = std::make_unique<AssetManager>();
	// Graphics
	graphicsManager = std::make_unique<Graphics::GraphicsManager>(appWindow);
	graphicsManager->Init();

	// Locators for global access
	IHCEngine::Core::AssetManagerLocator::Provide(assetManager.get());
	IHCEngine::Core::GraphicsManagerLocator::Provide(graphicsManager.get());

	// Application
	application->Init();
}

void IHCEngine::Core::Engine::Update()
{
	while (!appWindow->ShouldClose())
	{
		Time::GetInstance().Update();
		application->Update();

		// Time::Reset(); // if scene change

		while (Time::GetInstance().ShouldExecuteFixedUpdate())
		{
			Time::GetInstance().UpdateFixedTime();
			//component->FixedUpdate();
			//physics->Update();

			//sceneManager->Update();

		}

		graphicsManager->Update();
	}
}

void IHCEngine::Core::Engine::Shutdown()
{
	// order matters
	application->Shutdown();
	graphicsManager->Shutdown();
	assetManager = nullptr;
	graphicsManager = nullptr;
	IHCEngine::Core::GraphicsManagerLocator::Provide(nullptr);
	IHCEngine::Core::AssetManagerLocator::Provide(nullptr);
}
