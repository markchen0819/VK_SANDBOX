#include "SampleApplication.h"

#include "../../Engine/src/Core/Locator/Locators.h"
#include "SampleScene.h"

SampleApplication::SampleApplication::SampleApplication(const std::string& appName)
	:sceneNum{ 0 }, IHCEngine::Core::IApplication(appName)
{
}

void SampleApplication::SampleApplication::Init()
{
	// Get SceneManager
	auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
	sceneManager->AddScene<SampleScene>();
	sceneManager->SetNextSceneToLoad(0);
}

void SampleApplication::SampleApplication::Update()
{
	// Cheat Codes
	// AppWindow related updates
}

void SampleApplication::SampleApplication::Shutdown()
{


}
