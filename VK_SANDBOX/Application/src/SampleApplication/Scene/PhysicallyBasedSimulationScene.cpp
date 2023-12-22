#include "PhysicallyBasedSimulationScene.h"

// Engine resources
#include "../../Engine/src/Core/Locator/GraphicsManagerLocator.h"
#include "../../Engine/src/Core/Locator/AssetManagerLocator.h"
#include "../../Engine/src/Core/Time/Time.h"

// Components
#include "../CustomBehaviors/CameraController.h"
#include "../../../../Engine/src/Core/Scene/Components/MeshComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/TextureComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/ModelComponent.h"
#include "../CustomBehaviors/ImguiContext_PhysicallyBasedSimulationViewer.h"
#include "../CustomBehaviors/PhysicallyBasedSimulationViewer.h"


SampleApplication::PhysicallyBasedSimulationScene::PhysicallyBasedSimulationScene()
	: Scene("PhysicallyBasedSimulationScene")
{
}

void SampleApplication::PhysicallyBasedSimulationScene::Load()
{
	// Create Graphics resource using GraphicsAssetCreator
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	// viking Room
	auto sphereTexture =
		graphicsAssetCreator.CreateTexture("sphereTexture",
			"Application/assets/Models/Sphere/sphereTexture.jpg");
	auto sphereMesh =
		graphicsAssetCreator.CreateMesh("sphereMesh",
			"Application/assets/Models/Sphere/sphere.obj");

	createClothMeshAndLoadClothTexture();
	// x y z axis
	createAxisMeshAndLoadAxisTexture();
	// grid
	createGridMeshAndLoadGridTexture();
}

void SampleApplication::PhysicallyBasedSimulationScene::UnLoad()
{

	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	graphicsAssetCreator.DestroyMesh("clothMesh");
	graphicsAssetCreator.DestroyTexture("clothTexture");
	graphicsAssetCreator.DestroyMesh("sphereMesh");
	graphicsAssetCreator.DestroyTexture("sphereTexture");


	// x y z axis
	graphicsAssetCreator.DestroyTexture("plainTexture");
	graphicsAssetCreator.DestroyMesh("x_axisModel");
	graphicsAssetCreator.DestroyMesh("y_axisModel");
	graphicsAssetCreator.DestroyMesh("z_axisModel");

	// grid
	graphicsAssetCreator.DestroyTexture("gridTexture");
	graphicsAssetCreator.DestroyMesh("gridModel");

	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
	assetManager->ClearAllAssetRepositories();
}

void SampleApplication::PhysicallyBasedSimulationScene::Init()
{
	IHCEngine::Core::Time::LockFrameRate(144);

	IHCEngine::Core::GameObject& camera = AddGameObject("camera");
	//camera.AddComponent<SampleApplication::CameraController>();
	camera.AddComponent<PhysicallyBasedSimulationViewer>();
	camera.AddComponent<IHCEngine::Component::ImguiContext_PhysicallyBasedSimulationViewer>();

	//////////////////////////////////////////////////////////////////
	// GameObjects creation and component adding here
	//////////////////////////////////////////////////////////////////

	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
	IHCEngine::Component::MeshComponent* meshcomponent = nullptr;
	IHCEngine::Component::TextureComponent* texturecomponent = nullptr;
	IHCEngine::Component::ModelComponent* modelcomponent = nullptr;

	IHCEngine::Core::GameObject& clothGobj = AddGameObject("cloth");
	meshcomponent = clothGobj.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("clothMesh"));
	texturecomponent = clothGobj.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("clothTexture"));
	clothGobj.transform.SetPosition(glm::vec3(0, 0, 0));
	// Create clothGobj vertices in global space, set as origin
	//clothGobj.transform.SetPosition(glm::vec3(0, 10, 0));
	//clothGobj.transform.SetRotation(glm::vec3(0, 180, 0));

	IHCEngine::Core::GameObject& sphereGobj = AddGameObject("sphere");
	meshcomponent = sphereGobj.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("sphereMesh"));
	texturecomponent = sphereGobj.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("sphereTexture"));
	sphereGobj.transform.SetPosition(glm::vec3(0, 3, 0));

	///////////////////////////
	IHCEngine::Core::GameObject& x_axis = AddGameObject("x_axis");
	meshcomponent = x_axis.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("x_axisModel"));
	texturecomponent = x_axis.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("plainTexture"));
	x_axis.transform.SetPosition(glm::vec3(1, 0, 0));

	IHCEngine::Core::GameObject& y_axis = AddGameObject("y_axis");
	meshcomponent = y_axis.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("y_axisModel"));
	texturecomponent = y_axis.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("plainTexture"));
	y_axis.transform.SetRotation(glm::vec3(0, 0, 90));
	y_axis.transform.SetPosition(glm::vec3(0, 1, 0));

	IHCEngine::Core::GameObject& z_axis = AddGameObject("z_axis");
	meshcomponent = z_axis.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("z_axisModel"));
	texturecomponent = z_axis.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("plainTexture"));
	z_axis.transform.SetRotation(glm::vec3(0, 90, 0));
	z_axis.transform.SetPosition(glm::vec3(0, 0, 1));

	IHCEngine::Core::GameObject& grid = AddGameObject("grid");
	meshcomponent = grid.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("gridModel"));
	texturecomponent = grid.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("gridTexture"));
	grid.transform.SetPosition(glm::vec3(0, -0.01, 0));
	grid.transform.SetRotation(glm::vec3(180, 0, 0));
}

void SampleApplication::PhysicallyBasedSimulationScene::Reset()
{

}

void SampleApplication::PhysicallyBasedSimulationScene::createClothMeshAndLoadClothTexture()
{
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	auto clothTexture = graphicsAssetCreator.CreateTexture("clothTexture", "Application/assets/Textures/clothTexture.png");

	IHCEngine::Graphics::IHCMesh::Builder clothBuilder;
	int gridWidth = 35; 
	int gridHeight = 35;
	float clothSpacing = 0.1f; 

	// Calculate the offset to center the grid
	float xOffset = (gridWidth - 1) * clothSpacing / 2.0f;
	float yOffset = (gridHeight - 1) * clothSpacing / 2.0f;

	for (int y = 0; y < gridHeight; ++y) 
	{
		for (int x = 0; x < gridWidth; ++x) 
		{
			Vertex vertex;
			vertex.position = glm::vec3(x * clothSpacing - xOffset, 10, y * clothSpacing - yOffset);
			//vertex.position = glm::vec3(x * clothSpacing - xOffset, y * clothSpacing - yOffset, 0);
			vertex.uv = glm::vec2(float(x) / (gridWidth - 1), float(y) / (gridHeight - 1)); // UV coordinates
			vertex.normal = glm::vec3(0, 1, 0);  // up
			vertex.color = glm::vec3(1.0f); // blue color, modify as needed
			clothBuilder.vertices.push_back(vertex);
		}
	}

	for (int y = 0; y < gridHeight - 1; ++y) 
	{
		for (int x = 0; x < gridWidth - 1; ++x) 
		{
			// Get the indices of the four corners of the square
			uint32_t topLeft = y * gridWidth + x;
			uint32_t topRight = topLeft + 1;
			uint32_t bottomLeft = (y + 1) * gridWidth + x;
			uint32_t bottomRight = bottomLeft + 1;

			clothBuilder.indices.push_back(topLeft);
			clothBuilder.indices.push_back(bottomLeft);
			clothBuilder.indices.push_back(bottomRight);
			clothBuilder.indices.push_back(topLeft);
			clothBuilder.indices.push_back(bottomRight);
			clothBuilder.indices.push_back(topRight);
		}
	}

	auto clothMesh = graphicsAssetCreator.CreateMesh("clothMesh", clothBuilder);
}

void SampleApplication::PhysicallyBasedSimulationScene::createGridMeshAndLoadGridTexture()
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

void SampleApplication::PhysicallyBasedSimulationScene::createAxisMeshAndLoadAxisTexture()
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
