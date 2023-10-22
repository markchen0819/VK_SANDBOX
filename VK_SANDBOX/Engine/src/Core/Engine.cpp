#include "../pch.h"
#include "Engine.h"

#include "Time/Time.h"
#include "../Input/Input.h"
#include "../Window/AppWindow.h"
#include "Asset/AssetManager.h"
#include "Scene/SceneManager.h"
#include "../Graphics/GraphicsManager.h"
#include "../Imgui/ImGuiManager.h"

//Locators
#include "Locator/AppWindowLocator.h"
#include "Locator/AssetManagerLocator.h"
#include "Locator/GraphicsManagerLocator.h"
#include "Locator/SceneManagerLocator.h"
#include "Locator/CustomBehaviorManagerLocator.h"
#include "Locator/ImGuiManagerLocator.h"

//STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
// tiny loader
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>




// explicitly call Init & Shutdown
// = default is needed in cpp due to forward declaration for smart ptrs
IHCEngine::Core::Engine::Engine() = default;
IHCEngine::Core::Engine::~Engine() = default;

void IHCEngine::Core::Engine::Init()
{
	if (application == nullptr)
	{
		std::cerr << "Engine application can't be null" << std::endl;
		assert(false);
	}

	// Window
	appWindow = std::make_unique<Window::AppWindow>(application->GetName(), 1280, 720);
	// Input
	IHCEngine::Core::Input::Init();
	// Time
	IHCEngine::Core::Time::Init();
	Time::LockFrameRate(120);
	Time::SetFixedTime(Time::FIXED_UPDATE_TIME_STEP);
	// Asset
	assetManager = std::make_unique<AssetManager>();
	// Scene
	sceneManager = std::make_unique<SceneManager>();
	// Graphics
	graphicsManager = std::make_unique<Graphics::GraphicsManager>(appWindow);
	graphicsManager->Init();
	// Custom Behaviors
	customBehaviorManager = std::make_unique<Component::CustomBehaviorManager>();
	// IMGUI
	imguiManager = std::make_unique<IMGUI::ImGuiManager>();

	// Locators for global access
	IHCEngine::Core::AppWindowLocator::Provide(appWindow.get());
	IHCEngine::Core::AssetManagerLocator::Provide(assetManager.get());
	IHCEngine::Core::SceneManagerLocator::Provide(sceneManager.get());
	IHCEngine::Core::GraphicsManagerLocator::Provide(graphicsManager.get());
	IHCEngine::Core::CustomBehaviorManagerLocator::Provide(customBehaviorManager.get());
	IHCEngine::Core::ImGuiManagerLocator::Provide(imguiManager.get());
	
	imguiManager->Init(); // window & graphics locator required
	// Application
	application->Init();
}

void IHCEngine::Core::Engine::Update()
{
	while (!appWindow->ShouldClose())
	{
		if (!appWindow->IsWindowFocused())
		{
			glfwWaitEvents(); // Wait for an event instead of polling
			continue; 
		}

		Time::Update();

		application->Update();

		if (sceneManager->ShouldLoadNextScene())
		{
			customBehaviorManager->Reset();
			sceneManager->LoadNextScene();
			customBehaviorManager->Init();
			Input::Reset();
			Time::Reset();
		}

		// FixedUpdate
		while (Time::ShouldExecuteFixedUpdate())
		{
			Time::UpdateFixedTime();
			customBehaviorManager->FixedUpdate();
			//physics->Update();
			sceneManager->Update();
		}
		// Update
		if (Time::ShouldExecuteUpdate())
		{
			imguiManager->NewFrame();
			customBehaviorManager->Update();
			sceneManager->Update();
			graphicsManager->Update(sceneManager->GetActiveScene());
			Input::Update();
			glfwPollEvents();
		}
		sceneManager->DeferDestroyGameObjects();
	}
}

void IHCEngine::Core::Engine::Shutdown()
{
	// order matters
	application->Shutdown();
	imguiManager->ShutDown();
	sceneManager->Shutdown();
	graphicsManager->Shutdown();

	customBehaviorManager = nullptr;
	imguiManager = nullptr;
	sceneManager = nullptr;
	graphicsManager = nullptr;
	assetManager = nullptr;


	IHCEngine::Core::CustomBehaviorManagerLocator::Provide(nullptr);
	IHCEngine::Core::GraphicsManagerLocator::Provide(nullptr);
	IHCEngine::Core::SceneManagerLocator::Provide(nullptr);
	IHCEngine::Core::AssetManagerLocator::Provide(nullptr);
	IHCEngine::Core::AppWindowLocator::Provide(nullptr);	
}
