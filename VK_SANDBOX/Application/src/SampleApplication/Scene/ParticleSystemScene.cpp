#include "ParticleSystemScene.h"

// Engine resources
#include "../../Engine/src/Core/Locator/GraphicsManagerLocator.h"
#include "../../Engine/src/Core/Locator/AssetManagerLocator.h"
#include "../../Engine/src/Core/Time/Time.h"

// Components
#include "../CustomBehaviors/CameraController.h"
#include "../../../../Engine/src/Core/Scene/Components/MeshComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/TextureComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/ComputeParticleComponent.h"
#include "../CustomBehaviors/ImguiContext_ParticleSystemScene.h"

namespace IHCEngine::Component
{
	class ModelComponent;
}

SampleApplication::ParticleSystemScene::ParticleSystemScene()
	: Scene("ParticleSystemScene")
{
}

void SampleApplication::ParticleSystemScene::Load()
{
	// Create Graphics resource using GraphicsAssetCreator
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	auto sphereMesh =
		graphicsAssetCreator.CreateMesh("sphereMesh",
			"Application/assets/Models/Sphere/sphere.obj");

	// x y z axis
	createAxisMeshAndLoadAxisTexture();
	// grid
	createGridMeshAndLoadGridTexture();
}

void SampleApplication::ParticleSystemScene::UnLoad()
{
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	graphicsAssetCreator.DestroyMesh("cubeMesh");
	graphicsAssetCreator.DestroyMesh("sphereMesh");

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

void SampleApplication::ParticleSystemScene::Init()
{
	IHCEngine::Core::Time::LockFrameRate(144);

	IHCEngine::Core::GameObject& camera = AddGameObject("camera");
	camera.AddComponent<SampleApplication::CameraController>();
	camera.AddComponent<IHCEngine::Component::ImguiContext_ParticleSystemScene>();
	//////////////////////////////////////////////////////////////////
	// GameObjects creation and component adding here
	//////////////////////////////////////////////////////////////////

	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
	IHCEngine::Component::MeshComponent* meshcomponent = nullptr;
	IHCEngine::Component::TextureComponent* texturecomponent = nullptr;


	// Ch44
	IHCEngine::Core::GameObject& particleGobj = AddGameObject("particleGobj");
	particleGobj.AddComponent<IHCEngine::Component::ComputeParticleComponent>();
	particleGobj.transform.SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));


	//IHCEngine::Graphics::IHCMesh::Builder cubeBuilder;
	//cubeBuilder.vertices = {
	//	// Front face
	//	{{-1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}}, // Bottom-left
	//	{{ 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}}, // Bottom-right
	//	{{ 1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}}, // Top-right
	//	{{-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}}, // Top-left
	//	// Back face
	//	{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}}, // Bottom-left
	//	{{ 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}}, // Bottom-right
	//	{{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}}, // Top-right
	//	{{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}}  // Top-left
	//};
	//cubeBuilder.indices = {
	//	// Front face
	//	0, 1, 2,  0, 2, 3,
	//	// Right face
	//	1, 5, 6,  1, 6, 2,
	//	// Back face
	//	5, 4, 7,  5, 7, 6,
	//	// Left face
	//	4, 0, 3,  4, 3, 7,
	//	// Top face
	//	3, 2, 6,  3, 6, 7,
	//	// Bottom face
	//	1, 0, 4,  1, 4, 5
	//};

	IHCEngine::Graphics::IHCMesh::Builder cubeBuilder;
	cubeBuilder.vertices = {
		// Front face
		{{-1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Bottom-left
		{{ 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Bottom-right
		{{ 1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Top-right
		{{-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Top-left

		// Right face
		{{ 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // Bottom-left
		{{ 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // Bottom-right
		{{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // Top-right
		{{ 1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // Top-left

		// Back face
		{{ 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}, // Bottom-left
		{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}, // Bottom-right
		{{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}, // Top-right
		{{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}}, // Top-left

		// Left face
		{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}, // Bottom-left
		{{-1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}, // Bottom-right
		{{-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}, // Top-right
		{{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}}, // Top-left

		// Top face
		{{-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Bottom-left
		{{ 1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Bottom-right
		{{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Top-right
		{{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // Top-left

		// Bottom face
		{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}, // Bottom-left
		{{ 1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}, // Bottom-right
		{{ 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}, // Top-right
		{{-1.0f, -1.0f,  1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}  // Top-left
	};

	cubeBuilder.indices = {
		// Front face
		0, 1, 2,  0, 2, 3,
		// Right face
		4, 5, 6,  4, 6, 7,
		// Back face
		8, 9, 10, 8, 10, 11,
		// Left face
		12, 13, 14, 12, 14, 15,
		// Top face
		16, 17, 18, 16, 18, 19,
		// Bottom face
		20, 21, 22, 20, 22, 23
	};


	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();
	auto cubeMesh = graphicsAssetCreator.CreateMesh(
		"cubeMesh", cubeBuilder);

	meshcomponent = particleGobj.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("cubeMesh"));
	texturecomponent = particleGobj.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("plainTexture"));




	///////////////////////////
	// Others

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

void SampleApplication::ParticleSystemScene::Reset()
{

}

void SampleApplication::ParticleSystemScene::createGridMeshAndLoadGridTexture()
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
void SampleApplication::ParticleSystemScene::createAxisMeshAndLoadAxisTexture()
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
