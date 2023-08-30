#include "SampleScene.h"
#include "../../Engine/src/Core/Locator/Locators.h"

SampleApplication::SampleScene::SampleScene()
	: Scene("SampleScene")
{
}

void SampleApplication::SampleScene::Load()
{
	// Create Texture & Model functions belongs to Graphics Manager 
	// as it requires Vulkan parts (vkdevice, vkdesciptorsets...)
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();

	auto texture1 = graphicsManager->CreateTexture("roomNormal", "Engine/assets/models/viking_room/viking_room.png");
	auto texture2 = graphicsManager->CreateTexture("roomPink", "Engine/assets/models/viking_room/viking_room_2.png");
	auto model1 = graphicsManager->CreateModel("roomModel", "Engine/assets/models/viking_room/viking_room.obj");

	// But let AssetManager to manage the life time of the resources
	assetManager->GetTextureRepository().AddAsset("roomNormal", std::move(texture1));
	assetManager->GetTextureRepository().AddAsset("roomPink", std::move(texture2));
	assetManager->GetModelRepository().AddAsset("roomModel", std::move(model1));
}

void SampleApplication::SampleScene::UnLoad()
{
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();

	// release Vulkan parts for reuse (vkdesciptorsets...)
	graphicsManager->DestroyTexture("roomNormal");
	graphicsManager->DestroyTexture("roomPink");
	graphicsManager->DestroyModel("roomModel");

	// end of resource lifetime
	assetManager->GetTextureRepository().RemoveAsset("roomNormal");
	assetManager->GetTextureRepository().RemoveAsset("roomPink");
	assetManager->GetModelRepository().RemoveAsset("roomModel");
}

void SampleApplication::SampleScene::Init()
{
	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();

	IHCEngine::Core::GameObject& testGobj1 = AddGameObject("testGobj1");
	testGobj1.model = assetManager->GetModelRepository().GetAsset("roomModel");
	testGobj1.texture = assetManager->GetTextureRepository().GetAsset("roomNormal"); 

	IHCEngine::Core::GameObject& testGobj2 = AddGameObject("testGobj2");
	testGobj2.model = assetManager->GetModelRepository().GetAsset("roomModel");
	testGobj2.texture = assetManager->GetTextureRepository().GetAsset("roomPink");
}

void SampleApplication::SampleScene::Reset()
{

}
