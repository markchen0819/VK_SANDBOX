#include "SampleApplication.h"

#include "../../../Engine/src/Input/Input.h"
#include "../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "Scene/MotionAlongPathScene.h"
#include "Scene/SkeletalAnimationScene.h"

SampleApplication::SampleApplication::SampleApplication(const std::string& appName)
	:sceneNum{ 0 }, IHCEngine::Core::IApplication(appName)
{
}

void SampleApplication::SampleApplication::Init()
{
	// Get SceneManager
	auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
	sceneManager->AddScene<SkeletalAnimationScene>();
	sceneManager->AddScene<MotionAlongPathScene>();
	sceneManager->SetNextSceneToLoad(1);
}

void SampleApplication::SampleApplication::Update()
{
	// Cheat Codes
	// AppWindow related updates

	auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
	if(IHCEngine::Core::Input::IsKeyUp(GLFW_KEY_F1))
	{
		std::cout <<"=========================================" <<std::endl;
		std::cout << "==========    Loading Scene 0   ========" << std::endl;
		std::cout << "=========================================" << std::endl;
		sceneManager->SetNextSceneToLoad(0);
	}
	if (IHCEngine::Core::Input::IsKeyUp(GLFW_KEY_F2))
	{
		std::cout << "=========================================" << std::endl;
		std::cout << "==========    Loading Scene 1   ========" << std::endl;
		std::cout << "=========================================" << std::endl;
		sceneManager->SetNextSceneToLoad(1);
	}
}

void SampleApplication::SampleApplication::Shutdown()
{


}
