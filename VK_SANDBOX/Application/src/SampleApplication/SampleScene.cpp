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

	auto texture1 = graphicsManager->CreateTexture("room_TEX", "Engine/assets/textures/viking_room/viking_room.png");
	auto texture2 = graphicsManager->CreateTexture("pinkroom_TEX", "Engine/assets/textures/viking_room/viking_room_pink.png");
	auto model1 = graphicsManager->CreateModel("room_MODEL", "Engine/assets/models/viking_room/viking_room.obj");

	// But let AssetManager to manage the life time of the resources
	assetManager->GetTextureRepository().AddAsset("room_TEX", std::move(texture1));
	assetManager->GetTextureRepository().AddAsset("pinkroom_TEX", std::move(texture2));
	assetManager->GetModelRepository().AddAsset("room_MODEL", std::move(model1));
}

void SampleApplication::SampleScene::UnLoad()
{
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();

	// release Vulkan parts for reuse (vkdesciptorsets...)
	graphicsManager->DestroyTexture("room_TEX");
	graphicsManager->DestroyTexture("pinkroom_TEX");
	graphicsManager->DestroyModel("room_MODEL");

	// end of resource lifetime
	assetManager->GetTextureRepository().RemoveAsset("room_TEX");
	assetManager->GetTextureRepository().RemoveAsset("pinkroom_TEX");
	assetManager->GetModelRepository().RemoveAsset("room_MODEL");
}

void SampleApplication::SampleScene::Init()
{
	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();

	IHCEngine::Core::GameObject& testGobj1 = AddGameObject("testGobj1");
	testGobj1.model = assetManager->GetModelRepository().GetAsset("room_MODEL");
	testGobj1.texture = assetManager->GetTextureRepository().GetAsset("room_TEX"); 

	IHCEngine::Core::GameObject& testGobj2 = AddGameObject("testGobj2");
	testGobj2.model = assetManager->GetModelRepository().GetAsset("room_MODEL");
	testGobj2.texture = assetManager->GetTextureRepository().GetAsset("pinkroom_TEX");

	//mainCamera.SetAspectRatio(800, 600); // TO:DO get from window
	//glm::vec3 eyePosition = glm::vec3(5.0f, 5.0f, 5.0f);
	//glm::vec3 targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	//glm::vec3 upVector = glm::vec3(0.0f, 0.0f, 1.0f);
	//mainCamera.transform.SetWorldPosition(eyePosition);
	//glm::vec3 direction = glm::normalize(targetPosition - eyePosition);
	//glm::quat rotation = glm::quatLookAt(direction, upVector);  // Generate the quaternion rotation from the look direction and the up vector
	//mainCamera.transform.SetWorldRotation(rotation);  // Set the camera's rotation
}

void SampleApplication::SampleScene::Reset()
{

}
