#include "SampleScene.h"
#include "../../Engine/src/Core/Locator/GraphicsManagerLocator.h"
#include "../../Engine/src/Core/Locator/AssetManagerLocator.h"

// Components
#include "../CustomBehaviors/CameraController.h"
#include "../../Engine/src/Graphics/Animation/Model.h"
#include "../../../../Engine/src/Core/Scene/Components/MeshComponent.h"

SampleApplication::SampleScene::SampleScene()
	: Scene("SampleScene")
{
}

void SampleApplication::SampleScene::Load()
{
	// Create Graphics resource using GraphicsAssetCreator
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	// testmodel
	auto testModel = graphicsAssetCreator.CreateModel("testModel",
		//"Application/assets/Test/mococo-abyssgard/Mococo_pose.fbx");
		//"Application/assets/Test/X Bot.fbx");
		"Application/assets/Test/Bot/Ch44_nonPBR.fbx");

	IHCEngine::Graphics::Model* testModelRawPtr = testModel;

	auto testAnimation = std::make_unique<IHCEngine::Graphics::Animation>(
		"Application/assets/Test/Bot/Crouch To Stand.fbx", testModelRawPtr);

	// viking Room
	auto roomTexture = 
		graphicsAssetCreator.CreateTexture("roomTexture",
			"Engine/assets/textures/viking_room/viking_room.png");
	auto roomModel = 
		graphicsAssetCreator.CreateMesh("roomModel",
			"Engine/assets/models/viking_room/viking_room.obj");

	// x y z axis
	createAxisMeshAndLoadAxisTexture();
	
	// grid
	createGridMeshAndLoadGridTexture();

}

void SampleApplication::SampleScene::UnLoad()
{
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	// testModel
	graphicsAssetCreator.DestroyModel("testModel");
	//assetManager->GetModelRepository().RemoveAsset("testModel");
	//assetManager->GetAnimationRepository().RemoveAsset("testAnimation");

	// viking Room
	graphicsAssetCreator.DestroyTexture("roomTexture");
	graphicsAssetCreator.DestroyMesh("roomModel");

	// x y z axis
	graphicsAssetCreator.DestroyTexture("plainTexture");
	graphicsAssetCreator.DestroyMesh("x_axisModel");
	graphicsAssetCreator.DestroyMesh("y_axisModel");
	graphicsAssetCreator.DestroyMesh("z_axisModel");

	// grid
	graphicsAssetCreator.DestroyTexture("gridTexture");
	graphicsAssetCreator.DestroyMesh("gridModel");
}

void SampleApplication::SampleScene::Init()
{

	//////////////////////////////////////////////////////////////////
	// GameObjects creation and component adding here
	//////////////////////////////////////////////////////////////////

	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
	IHCEngine::Component::MeshComponent* meshcomponent = nullptr;


	IHCEngine::Core::GameObject& camera = AddGameObject("camera");
	camera.AddComponent<SampleApplication::CameraController>();

	IHCEngine::Core::GameObject& testModel = AddGameObject("testModel");
	testModel.model = assetManager->GetModelRepository().GetAsset("testModel");
	//auto animation = assetManager->GetAnimationRepository().GetAsset("testAnimation");
	//testModel.animator.PlayAnimation(animation);



	IHCEngine::Core::GameObject& room = AddGameObject("room");
	meshcomponent = room.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("roomModel"));
	room.texture = assetManager->GetTextureRepository().GetAsset("roomTexture");
	room.transform.SetPosition(glm::vec3(2.0f, 0.01f, 2.0f));
	room.transform.SetRotation(glm::vec3(-90, -90, 0));
	room.transform.SetScale(glm::vec3(1.5, 1.5, 1.5));

	IHCEngine::Core::GameObject& x_axis = AddGameObject("x_axis");
	meshcomponent = x_axis.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("x_axisModel"));
	x_axis.texture = assetManager->GetTextureRepository().GetAsset("plainTexture");
	x_axis.transform.SetPosition(glm::vec3(1, 0, 0));

	IHCEngine::Core::GameObject& y_axis = AddGameObject("y_axis");
	meshcomponent = y_axis.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("y_axisModel"));
	y_axis.texture = assetManager->GetTextureRepository().GetAsset("plainTexture");
	y_axis.transform.SetRotation(glm::vec3(0, 0, 90));
	y_axis.transform.SetPosition(glm::vec3(0, 1, 0));

	IHCEngine::Core::GameObject& z_axis = AddGameObject("z_axis");
	meshcomponent = z_axis.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("z_axisModel"));
	z_axis.texture = assetManager->GetTextureRepository().GetAsset("plainTexture");
	z_axis.transform.SetRotation(glm::vec3(0, 90, 0));
	z_axis.transform.SetPosition(glm::vec3(0, 0, 1));

	IHCEngine::Core::GameObject& grid = AddGameObject("grid");
	meshcomponent = grid.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("gridModel"));
	grid.texture = assetManager->GetTextureRepository().GetAsset("gridTexture");
	grid.transform.SetPosition(glm::vec3(0, -0.01, 0));

}

void SampleApplication::SampleScene::Reset()
{

}

void SampleApplication::SampleScene::createGridMeshAndLoadGridTexture()
{
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	auto gridTexture = graphicsAssetCreator.CreateTexture("gridTexture", "Engine/assets/textures/Grid.png");
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
	auto gridModel = graphicsAssetCreator.CreateMesh("gridModel", gridBuilder);
}
void SampleApplication::SampleScene::createAxisMeshAndLoadAxisTexture()
{
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

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
	auto x_axisModel = graphicsAssetCreator.CreateMesh(
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
	auto y_axisModel = graphicsAssetCreator.CreateMesh(
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
	auto z_axisModel = graphicsAssetCreator.CreateMesh(
		"z_axisModel", axisBuilder);

	auto plainTexture = graphicsAssetCreator.CreateTexture("plainTexture", "Engine/assets/textures/Plain.png");
}
