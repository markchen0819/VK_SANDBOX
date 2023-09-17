#include "SampleScene.h"
#include "../../Engine/src/Core/Locator/GraphicsManagerLocator.h"
#include "../../Engine/src/Core/Locator/AssetManagerLocator.h"
#include "../CustomBehaviors/CameraController.h"

#include "../../Engine/src/Graphics/Animation/Model.h"

SampleApplication::SampleScene::SampleScene()
	: Scene("SampleScene")
{
}

void SampleApplication::SampleScene::Load()
{
	// Create Texture & Model functions belongs to Graphics Manager 
	// as it requires Vulkan parts (vkdevice, vkdesciptorsets...
	//////////////////////////////////////////////////////////////////
	// STEP1: Create textures and models using GraphicsManager
	// STEP2: Move resources to AssetManager to  manage life time of the resources
	//////////////////////////////////////////////////////////////////
	
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();

	// testmodel
	auto testModel = graphicsManager->CreateModel("testModel",
		//"Application/assets/Test/mococo-abyssgard/Mococo_pose.fbx");
		//"Application/assets/Test/X Bot.fbx");
		"Application/assets/Test/Bot/Ch44_nonPBR.fbx");
	
	assetManager->GetModelRepository().AddAsset("testModel",
		std::move(testModel));

	// viking Room
	auto roomTexture = 
		graphicsManager->CreateTexture("roomTexture",
			"Engine/assets/textures/viking_room/viking_room.png");
	auto roomModel = 
		graphicsManager->CreateMesh("roomModel",
			"Engine/assets/models/viking_room/viking_room.obj");
	assetManager->GetTextureRepository().AddAsset("roomTexture",
		std::move(roomTexture));
	assetManager->GetMeshRepository().AddAsset("roomModel", 
		std::move(roomModel));

	// x y z axis
	IHCEngine::Graphics::IHCMesh::Builder axisBuilder;
	axisBuilder.vertices = {
		{{ 1.0f,  0.1f,  0.1f}, {1.0f, 0.0f, 0.0f}},
		{{ 1.0f, -0.1f,  0.1f}, {1.0f, 0.0f, 0.0f}},
		{{ 1.0f, -0.1f, -0.1f}, {1.0f, 0.0f, 0.0f}},
		{{ 1.0f,  0.1f, -0.1f}, {1.0f, 0.0f, 0.0f}},
		{{-1.0f,  0.1f,  0.1f}, {1.0f, 0.0f, 0.0f}},
		{{-1.0f, -0.1f,  0.1f}, {1.0f, 0.0f, 0.0f}},
		{{-1.0f, -0.1f, -0.1f}, {1.0f, 0.0f, 0.0f}},
		{{-1.0f,  0.1f, -0.1f}, {1.0f, 0.0f, 0.0f}}
	};
	axisBuilder.indices = {
		0, 1, 5,  0, 5, 4,
		1, 2, 6,  1, 6, 5,
		2, 3, 7,  2, 7, 6,
		3, 0, 4,  3, 4, 7,
		4, 5, 6,  4, 6, 7,
		0, 3, 2,  0, 2, 1
	};
	auto x_axisModel = graphicsManager->CreateMesh(
		"x_axisModel", axisBuilder);
	axisBuilder.vertices = {
	{{ 1.0f,  0.1f,  0.1f}, {0.0f, 1.0f, 0.0f}},
	{{ 1.0f, -0.1f,  0.1f}, {0.0f, 1.0f, 0.0f}},
	{{ 1.0f, -0.1f, -0.1f}, {0.0f, 1.0f, 0.0f}},
	{{ 1.0f,  0.1f, -0.1f}, {0.0f, 1.0f, 0.0f}},
	{{-1.0f,  0.1f,  0.1f}, {0.0f, 1.0f, 0.0f}},
	{{-1.0f, -0.1f,  0.1f}, {0.0f, 1.0f, 0.0f}},
	{{-1.0f, -0.1f, -0.1f}, {0.0f, 1.0f, 0.0f}},
	{{-1.0f,  0.1f, -0.1f}, {0.0f, 1.0f, 0.0f}}
	};
	auto y_axisModel = graphicsManager->CreateMesh(
		"y_axisModel", axisBuilder);
	axisBuilder.vertices = {
	{{ 1.0f,  0.1f,  0.1f}, {0.0f, 0.0f, 1.0f}},
	{{ 1.0f, -0.1f,  0.1f}, {0.0f, 0.0f, 1.0f}},
	{{ 1.0f, -0.1f, -0.1f}, {0.0f, 0.0f, 1.0f}},
	{{ 1.0f,  0.1f, -0.1f}, {0.0f, 0.0f, 1.0f}},
	{{-1.0f,  0.1f,  0.1f}, {0.0f, 0.0f, 1.0f}},
	{{-1.0f, -0.1f,  0.1f}, {0.0f, 0.0f, 1.0f}},
	{{-1.0f, -0.1f, -0.1f}, {0.0f, 0.0f, 1.0f}},
	{{-1.0f,  0.1f, -0.1f}, {0.0f, 0.0f, 1.0f}}
	};
	auto z_axisModel = graphicsManager->CreateMesh(
		"z_axisModel", axisBuilder);

	assetManager->GetMeshRepository().AddAsset("x_axisModel",
		std::move(x_axisModel));
	assetManager->GetMeshRepository().AddAsset("y_axisModel",
		std::move(y_axisModel));
	assetManager->GetMeshRepository().AddAsset("z_axisModel",
		std::move(z_axisModel));

	auto plainTexture = graphicsManager->CreateTexture("plainTexture", "Engine/assets/textures/Plain.png");
	assetManager->GetTextureRepository().AddAsset("plainTexture", std::move(plainTexture));

	// grid
	auto gridTexture = graphicsManager->CreateTexture("gridTexture", "Engine/assets/textures/Grid.png");
	IHCEngine::Graphics::IHCMesh::Builder gridBuilder;
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
	auto gridModel = graphicsManager->CreateMesh("gridModel", gridBuilder);
	assetManager->GetTextureRepository().AddAsset("gridTexture", std::move(gridTexture));
	assetManager->GetMeshRepository().AddAsset("gridModel", std::move(gridModel));


}

void SampleApplication::SampleScene::UnLoad()
{
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();

	//////////////////////////////////////////////////////////////////
	// STEP1: release Vulkan parts for reuse (vkdesciptorsets...) in GraphicsManager
	// STEP2: End of resource lifetime in AssetManager 
	//////////////////////////////////////////////////////////////////

	// testModel
	graphicsManager->DestroyModel("testModel");
	assetManager->GetModelRepository().RemoveAsset("testModel");


	// viking Room
	graphicsManager->DestroyTexture("roomTexture");
	graphicsManager->DestroyMesh("roomModel");
	assetManager->GetTextureRepository().RemoveAsset("roomTexture");
	assetManager->GetMeshRepository().RemoveAsset("roomModel");


	// x y z axis
	graphicsManager->DestroyTexture("plainTexture");
	graphicsManager->DestroyMesh("x_axisModel");
	graphicsManager->DestroyMesh("y_axisModel");
	graphicsManager->DestroyMesh("z_axisModel");
	assetManager->GetTextureRepository().RemoveAsset("plainTexture");
	assetManager->GetMeshRepository().RemoveAsset("x_axisModel");
	assetManager->GetMeshRepository().RemoveAsset("y_axisModel");
	assetManager->GetMeshRepository().RemoveAsset("z_axisModel");

	// grid
	graphicsManager->DestroyTexture("gridTexture");
	graphicsManager->DestroyMesh("gridModel");
	assetManager->GetTextureRepository().RemoveAsset("gridTexture");
	assetManager->GetMeshRepository().RemoveAsset("gridModel");


}

void SampleApplication::SampleScene::Init()
{
	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();

	IHCEngine::Core::GameObject& camera = AddGameObject("camera");
	camera.AddComponent<SampleApplication::CameraController>();

	IHCEngine::Core::GameObject& testModel = AddGameObject("testModel");
	testModel.model = assetManager->GetModelRepository().GetAsset("testModel");


	IHCEngine::Core::GameObject& room = AddGameObject("room");
	room.mesh = assetManager->GetMeshRepository().GetAsset("roomModel");
	room.texture = assetManager->GetTextureRepository().GetAsset("roomTexture");
	room.transform.SetPosition(glm::vec3(2.0f, 0.01f, 2.0f));
	room.transform.SetRotation(glm::vec3(-90, -90, 0));
	room.transform.SetScale(glm::vec3(1.5, 1.5, 1.5));

	IHCEngine::Core::GameObject& x_axis = AddGameObject("x_axis");
	x_axis.mesh = assetManager->GetMeshRepository().GetAsset("x_axisModel");
	x_axis.texture = assetManager->GetTextureRepository().GetAsset("plainTexture");
	x_axis.transform.SetPosition(glm::vec3(1, 0, 0));

	IHCEngine::Core::GameObject& y_axis = AddGameObject("y_axis");
	y_axis.mesh = assetManager->GetMeshRepository().GetAsset("y_axisModel");
	y_axis.texture = assetManager->GetTextureRepository().GetAsset("plainTexture");
	y_axis.transform.SetRotation(glm::vec3(0, 0, 90));
	y_axis.transform.SetPosition(glm::vec3(0, 1, 0));

	IHCEngine::Core::GameObject& z_axis = AddGameObject("z_axis");
	z_axis.mesh = assetManager->GetMeshRepository().GetAsset("z_axisModel");
	z_axis.texture = assetManager->GetTextureRepository().GetAsset("plainTexture");
	z_axis.transform.SetRotation(glm::vec3(0, 90, 0));
	z_axis.transform.SetPosition(glm::vec3(0, 0, 1));

	IHCEngine::Core::GameObject& grid = AddGameObject("grid");
	grid.mesh = assetManager->GetMeshRepository().GetAsset("gridModel");
	grid.texture = assetManager->GetTextureRepository().GetAsset("gridTexture");
	grid.transform.SetPosition(glm::vec3(0, -0.01, 0));

}

void SampleApplication::SampleScene::Reset()
{

}
