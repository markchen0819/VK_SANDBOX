#include "TestApplication.h"
#include "../../Engine/src/Core/Locator/Locators.h"

TestApplication::TestApplication::TestApplication(const std::string& appName)
	:sceneNum{ 0 }, IHCEngine::Core::IApplication(appName)
{
}

void TestApplication::TestApplication::Init()
{
	// Get SceneManager
	// Add Scene
	// LoadScene

	// Move this when we have scene
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();

	// Create Texture & Model functions belongs to Graphics Manager 
	// as it requires Vulkan parts (vkdevice, vkdesciptorsets...)
	auto texture1 = graphicsManager->CreateTexture("roomNormal", "Engine/assets/models/viking_room/viking_room.png");
	auto texture2 = graphicsManager->CreateTexture("roomPink", "Engine/assets/models/viking_room/viking_room_2.png");
	// But let AssetManager to manage the life time of the resources
	assetManager->GetTextureRepository().AddAsset("roomNormal", std::move(texture1));
	assetManager->GetTextureRepository().AddAsset("roomPink", std::move(texture2));

	graphicsManager->LoadGameObjects();
}

void TestApplication::TestApplication::Update()
{
	// Cheat Codes
	// AppWindow related updates
}

void TestApplication::TestApplication::Shutdown()
{
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();

	// release Vulkan parts (vkdesciptorsets...)
	graphicsManager->DestroyTexture("roomNormal");
	graphicsManager->DestroyTexture("roomPink");
	// end of resource lifetime
	assetManager->GetTextureRepository().RemoveAsset("roomNormal");
	assetManager->GetTextureRepository().RemoveAsset("roomPink");

}
