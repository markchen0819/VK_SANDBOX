#include "SampleScene.h"
#include "../../Engine/src/Core/Locator/GraphicsManagerLocator.h"
#include "../../Engine/src/Core/Locator/AssetManagerLocator.h"
#include "../CustomBehaviors/CameraController.h"


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


	// RGB
	auto texture3 = graphicsManager->CreateTexture("R", "Engine/assets/textures/viking_room/R.png");
	auto texture4 = graphicsManager->CreateTexture("G", "Engine/assets/textures/viking_room/G.png");
	auto texture5 = graphicsManager->CreateTexture("B", "Engine/assets/textures/viking_room/B.png");
	assetManager->GetTextureRepository().AddAsset("R", std::move(texture3));
	assetManager->GetTextureRepository().AddAsset("G", std::move(texture4));
	assetManager->GetTextureRepository().AddAsset("B", std::move(texture5));

	// grid
	auto gridTexture = graphicsManager->CreateTexture("GridTexture", "Engine/assets/textures/viking_room/Grid.png");
	IHCEngine::Graphics::IHCModel::Builder gridBuilder;
	int gridSize = 100;
	float halfSize = gridSize / 2.0f;  // this represents half the width/length of the grid
	for (int i = 0; i <= gridSize; ++i)
	{
		for (int j = 0; j <= gridSize; ++j)
		{
			Vertex vertex;
			vertex.position = glm::vec3(i - halfSize, 0, j - halfSize);  // Shift by halfSize
			vertex.color = glm::vec3(1.0f);  // white color, modify as needed
			vertex.normal = glm::vec3(0, 1, 0);  // up, as it's a flat grid

			// Adjust the texture coordinates so that every square on the grid 
			// has a texture that starts from (0,0) to (1,1).
			vertex.uv = glm::vec2(i % 2, j % 2);

			gridBuilder.vertices.push_back(vertex);
		}
	}
	for (int i = 0; i < gridSize; ++i)
	{
		for (int j = 0; j < gridSize; ++j)
		{
			uint32_t topLeft = i * (gridSize + 1) + j;
			uint32_t bottomLeft = (i + 1) * (gridSize + 1) + j;
			uint32_t topRight = i * (gridSize + 1) + j + 1;
			uint32_t bottomRight = (i + 1) * (gridSize + 1) + j + 1;

			gridBuilder.indices.push_back(topLeft);
			gridBuilder.indices.push_back(bottomLeft);
			gridBuilder.indices.push_back(topRight);
			gridBuilder.indices.push_back(bottomLeft);
			gridBuilder.indices.push_back(bottomRight);
			gridBuilder.indices.push_back(topRight);
		}
	}
	auto gridModel = graphicsManager->CreateModel("GridModel", gridBuilder);
	assetManager->GetTextureRepository().AddAsset("GridTexture", std::move(gridTexture));
	assetManager->GetModelRepository().AddAsset("GridModel", std::move(gridModel));


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

	// RGB
	graphicsManager->DestroyTexture("R");
	graphicsManager->DestroyTexture("G");
	graphicsManager->DestroyTexture("B");
	assetManager->GetTextureRepository().RemoveAsset("R");
	assetManager->GetTextureRepository().RemoveAsset("G");
	assetManager->GetTextureRepository().RemoveAsset("B");

	//
	graphicsManager->DestroyTexture("GridTexture");
	graphicsManager->DestroyModel("GridModel");
	assetManager->GetTextureRepository().RemoveAsset("GridTexture");
	assetManager->GetModelRepository().RemoveAsset("GridModel");


}

void SampleApplication::SampleScene::Init()
{
	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();

	IHCEngine::Core::GameObject& camera = AddGameObject("camera");
	camera.AddComponent<SampleApplication::CameraController>();

	IHCEngine::Core::GameObject& testGobj1 = AddGameObject("testGobj1");
	testGobj1.model = assetManager->GetModelRepository().GetAsset("room_MODEL");
	testGobj1.texture = assetManager->GetTextureRepository().GetAsset("room_TEX"); 

	//IHCEngine::Core::GameObject& testGobj2 = AddGameObject("testGobj2");
	//testGobj2.model = assetManager->GetModelRepository().GetAsset("room_MODEL");
	//testGobj2.texture = assetManager->GetTextureRepository().GetAsset("pinkroom_TEX");
	//testGobj2.transform.SetLocalPosition(glm::vec3(1, 0, 0));

	IHCEngine::Core::GameObject& testGobj2 = AddGameObject("testGobj2");
	testGobj2.model = assetManager->GetModelRepository().GetAsset("room_MODEL");
	testGobj2.texture = assetManager->GetTextureRepository().GetAsset("R");
	testGobj2.transform.SetPosition(glm::vec3(1, 0, 0));

	IHCEngine::Core::GameObject& testGobj3 = AddGameObject("testGobj3");
	testGobj3.model = assetManager->GetModelRepository().GetAsset("room_MODEL");
	testGobj3.texture = assetManager->GetTextureRepository().GetAsset("G");
	testGobj3.transform.SetPosition(glm::vec3(0, 1, 0));

	IHCEngine::Core::GameObject& testGobj4 = AddGameObject("testGobj4");
	testGobj4.model = assetManager->GetModelRepository().GetAsset("room_MODEL");
	testGobj4.texture = assetManager->GetTextureRepository().GetAsset("B");
	testGobj4.transform.SetPosition(glm::vec3(0, 0, 1));

	//mainCamera.SetAspectRatio(800, 600); // TO:DO get from window
	//glm::vec3 eyePosition = glm::vec3(5.0f, 5.0f, 5.0f);
	//glm::vec3 targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	//glm::vec3 upVector = glm::vec3(0.0f, 0.0f, 1.0f);
	//mainCamera.transform.SetWorldPosition(eyePosition);
	//glm::vec3 direction = glm::normalize(targetPosition - eyePosition);
	//glm::quat rotation = glm::quatLookAt(direction, upVector);  // Generate the quaternion rotation from the look direction and the up vector
	//mainCamera.transform.SetWorldRotation(rotation);  // Set the camera's rotation


	IHCEngine::Core::GameObject& gridGobj = AddGameObject("gridGobj");
	gridGobj.model = assetManager->GetModelRepository().GetAsset("GridModel");
	gridGobj.texture = assetManager->GetTextureRepository().GetAsset("GridTexture");
	gridGobj.transform.SetPosition(glm::vec3(0, -0.1, 0));

}

void SampleApplication::SampleScene::Reset()
{

}
