#include "SampleApplication.h"

#include "../../../Engine/src/Input/Input.h"
#include "../../Engine/src/Core/Locator/SceneManagerLocator.h"

#include "Scene/PhysicallyBasedSimulationScene.h"
#include "Scene/InverseKinematicsScene.h"
#include "Scene/MotionAlongPathScene.h"
#include "Scene/ParticleSystemScene.h"
#include "Scene/SkeletalAnimationScene.h"
#include "Scene/GrassScene.h"
#include "Scene/FluidScene.h"

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
	sceneManager->AddScene<InverseKinematicsScene>();
	sceneManager->AddScene<PhysicallyBasedSimulationScene>();
	sceneManager->AddScene<ParticleSystemScene>();
	sceneManager->AddScene<GrassScene>();
	sceneManager->AddScene<FluidScene>();
	sceneManager->SetNextSceneToLoad(6);
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
	if (IHCEngine::Core::Input::IsKeyUp(GLFW_KEY_F3))
	{
		std::cout << "=========================================" << std::endl;
		std::cout << "==========    Loading Scene 3   ========" << std::endl;
		std::cout << "=========================================" << std::endl;
		sceneManager->SetNextSceneToLoad(2);
	}
	if (IHCEngine::Core::Input::IsKeyUp(GLFW_KEY_F4))
	{
		std::cout << "=========================================" << std::endl;
		std::cout << "==========    Loading Scene 4   ========" << std::endl;
		std::cout << "=========================================" << std::endl;
		sceneManager->SetNextSceneToLoad(3);
	}
	if (IHCEngine::Core::Input::IsKeyUp(GLFW_KEY_F5))
	{
		std::cout << "=========================================" << std::endl;
		std::cout << "==========    Loading Scene 5  ========" << std::endl;
		std::cout << "=========================================" << std::endl;
		sceneManager->SetNextSceneToLoad(4);
	}
	if (IHCEngine::Core::Input::IsKeyUp(GLFW_KEY_F6))
	{
		std::cout << "=========================================" << std::endl;
		std::cout << "==========    Loading Scene 6  ========" << std::endl;
		std::cout << "=========================================" << std::endl;
		sceneManager->SetNextSceneToLoad(5);
	}
	if (IHCEngine::Core::Input::IsKeyUp(GLFW_KEY_F7))
	{
		std::cout << "=========================================" << std::endl;
		std::cout << "==========    Loading Scene 7  ========" << std::endl;
		std::cout << "=========================================" << std::endl;
		sceneManager->SetNextSceneToLoad(6);
	}
}

void SampleApplication::SampleApplication::Shutdown()
{


}
