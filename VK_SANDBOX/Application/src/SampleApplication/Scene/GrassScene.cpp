#include "GrassScene.h"

// Engine resources
#include "../../Engine/src/Core/Locator/GraphicsManagerLocator.h"
#include "../../Engine/src/Core/Locator/AssetManagerLocator.h"
#include "../../Engine/src/Core/Time/Time.h"

// Components
#include "../CustomBehaviors/CameraController.h"
#include "../../../../Engine/src/Core/Scene/Components/MeshComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/TextureComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/ComputeGrassComponent.h"
#include "../CustomBehaviors/ImguiContext/ImguiContext_GrassScene.h"

SampleApplication::GrassScene::GrassScene()
	: Scene("GrassScene")
{
}

void SampleApplication::GrassScene::Load()
{
	// Create Graphics resource using GraphicsAssetCreator
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	auto& graphicsAssetCreator = graphicsManager->GetGraphicsAssetCreator();

	// x y z axis
	createAxisMeshAndLoadAxisTexture();
	// grid
	createGridMeshAndLoadGridTexture();
	//
	createDirt();

	graphicsManager->SetClearColor(glm::vec3(0, 0, 0));
}

void SampleApplication::GrassScene::UnLoad()
{
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	auto& graphicsAssetCreator = graphicsManager->GetGraphicsAssetCreator();

	graphicsManager->SetClearColor(glm::vec3(0, 0.7, 1.0));

	graphicsAssetCreator.DestroyMesh("grassMesh");
	graphicsAssetCreator.DestroyTexture("grassTexture");
	graphicsAssetCreator.DestroyTexture("noiseTexture");

	graphicsAssetCreator.DestroyTexture("dirtTexture");
	graphicsAssetCreator.DestroyMesh("dirtMesh");

	grassChunkGobjs.clear();

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

void SampleApplication::GrassScene::Init()
{
	IHCEngine::Core::Time::LockFrameRate(144);

	IHCEngine::Core::GameObject& camera = AddGameObject("camera");
	camera.AddComponent<SampleApplication::CameraController>();
	camera.AddComponent<IHCEngine::Component::ImguiContext_GrassScene>();
	//////////////////////////////////////////////////////////////////
	// GameObjects creation and component adding here
	//////////////////////////////////////////////////////////////////

	// Should Put in Load
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();
	auto noiseTexture = graphicsAssetCreator.CreateTexture("noiseTexture", "Engine/assets/textures/perlinNoiseTexture.png");
	auto grassTexture =
		graphicsAssetCreator.CreateTexture("grassTexture",
			"Engine/assets/textures/grassBlade/grassTexture.png");
	auto grassMesh =
		graphicsAssetCreator.CreateMesh("grassMesh",
			"Engine/assets/models/grassBlade/grassBlade.obj");



	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
	IHCEngine::Component::MeshComponent* meshcomponent = nullptr;
	IHCEngine::Component::TextureComponent* texturecomponent = nullptr;
	IHCEngine::Component::ComputeGrassComponent* computeGrassComponent = nullptr;

	//IHCEngine::Core::GameObject& grassGobj = AddGameObject("grassGobj");
	//computeGrassComponent = grassGobj.AddComponent<IHCEngine::Component::ComputeGrassComponent>();
	//computeGrassComponent->SetNoiseTexture(noiseTexture);
	//meshcomponent = grassGobj.AddComponent<IHCEngine::Component::MeshComponent>();
	//meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("grassMesh"));
	//texturecomponent = grassGobj.AddComponent<IHCEngine::Component::TextureComponent>();
	//texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("grassTexture"));


	//// test performance
	int gridSize = 5;
	float offset = 20.0;
	float gridOffset = (gridSize / 2.0f) * offset;
	glm::vec3 centerOffset(-gridOffset + offset / 2.0f, 0, -gridOffset + offset / 2.0f);
	for(int i=0; i< gridSize; ++i)
	{
		for(int j=0; j< gridSize; ++j)
		{
			std::string str = "grassGobj" + std::to_string(i) + std::to_string(j);
			IHCEngine::Core::GameObject& grassGobj = AddGameObject(str);
			glm::vec3 position = centerOffset + glm::vec3(i * offset, 0, j * offset);
			grassGobj.transform.SetPosition(position);
			computeGrassComponent = grassGobj.AddComponent<IHCEngine::Component::ComputeGrassComponent>();
			computeGrassComponent->SetNoiseTexture(noiseTexture);
			computeGrassComponent->SetChunkCoords(i, j, gridSize, gridSize);
			meshcomponent = grassGobj.AddComponent<IHCEngine::Component::MeshComponent>();
			meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("grassMesh"));
			texturecomponent = grassGobj.AddComponent<IHCEngine::Component::TextureComponent>();
			texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("grassTexture"));
			grassChunkGobjs.push_back(&grassGobj);
		}
	}

	///////////////////////////
	// Others
	IHCEngine::Core::GameObject& dirtGobj = AddGameObject("dirtGobj");
	meshcomponent = dirtGobj.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("dirtMesh"));
	texturecomponent = dirtGobj.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("dirtTexture"));


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
}

void SampleApplication::GrassScene::Reset()
{

}

void SampleApplication::GrassScene::createGridMeshAndLoadGridTexture()
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
void SampleApplication::GrassScene::createAxisMeshAndLoadAxisTexture()
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

void SampleApplication::GrassScene::createDirt()
{
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	auto dirtTexture =
		graphicsAssetCreator.CreateTexture("dirtTexture",
			"Engine/assets/textures/grassBlade/dirtTexture.png");

	IHCEngine::Graphics::IHCMesh::Builder dirtBuilder;
	int gridSize = 100;
	float halfSize = gridSize / 2.0f;  // this represents half the width/length of the grid
	std::vector<Vertex> vertices(4);
	// Bottom left
	vertices[0].position = glm::vec3(-halfSize, 0.001f, -halfSize);
	vertices[0].color = glm::vec3(1.0f);
	vertices[0].normal = glm::vec3(0, 1, 0);
	vertices[0].uv = glm::vec2(0.0f, 0.0f);
	// Bottom right
	vertices[1].position = glm::vec3(halfSize, 0.001f, -halfSize);
	vertices[1].color = glm::vec3(1.0f);
	vertices[1].normal = glm::vec3(0, 1, 0);
	vertices[1].uv = glm::vec2(1.0f, 0.0f);
	// Top left
	vertices[2].position = glm::vec3(-halfSize, 0.001f, halfSize);
	vertices[2].color = glm::vec3(1.0f);
	vertices[2].normal = glm::vec3(0, 1, 0);
	vertices[2].uv = glm::vec2(0.0f, 1.0f);
	// Top right
	vertices[3].position = glm::vec3(halfSize, 0.001f, halfSize);
	vertices[3].color = glm::vec3(1.0f);
	vertices[3].normal = glm::vec3(0, 1, 0);
	vertices[3].uv = glm::vec2(1.0f, 1.0f);

	std::vector<uint32_t> indices = {
		0, 2, 1,
		1, 2, 3  
	};

	dirtBuilder.vertices = vertices;
	dirtBuilder.indices = indices;
	auto dirtMesh = graphicsAssetCreator.CreateMesh("dirtMesh", dirtBuilder);

}
