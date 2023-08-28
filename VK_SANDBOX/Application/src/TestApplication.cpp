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

	assetManager->GetTextureRepository().AddAsset("roomNormal",
		graphicsManager->CreateTexture("Engine/assets/models/viking_room/viking_room.png"));
	assetManager->GetTextureRepository().AddAsset("roomPink",
		graphicsManager->CreateTexture("Engine/assets/models/viking_room/viking_room_2.png"));

	graphicsManager->CreateLocalDescriptorSets(assetManager->GetTextureRepository().GetAssets());
}

void TestApplication::TestApplication::Update()
{
	// Cheat Codes
	// AppWindow related updates
}

void TestApplication::TestApplication::Shutdown()
{
}
