#include "InverseKinematicsScene.h"


// Engine resources
#include "../../Engine/src/Core/Locator/GraphicsManagerLocator.h"
#include "../../Engine/src/Core/Locator/AssetManagerLocator.h"
#include "../../Engine/src/Core/Time/Time.h"

// Components
#include "../CustomBehaviors/CameraController.h"
#include "../../../../Engine/src/Core/Scene/Components/MeshComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/TextureComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/ModelComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/AnimatorComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/PipelineComponent.h"

// Custom Behaviors
#include "../CustomBehaviors/InverseKinematicsViewer.h"
#include "../CustomBehaviors/ImguiContext_InverseKinematicsViewer.h"

SampleApplication::InverseKinematicsScene::InverseKinematicsScene()
	: Scene("InverseKinematicsScene")
{
}

void SampleApplication::InverseKinematicsScene::Load()
{
	// Create Graphics resource using GraphicsAssetCreator
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	// IK
	auto IKModel = graphicsAssetCreator.CreateModel("IKModel",
		"Application/assets/Models/IKModel/IK.fbx");
	createTargetObjectMesh();
	//// Animation
	//auto CrouchAnimation = graphicsAssetCreator.CreateAnimation(
	//	"CrouchAnimation", "Application/assets/Animations/Crouch To Stand.fbx",
	//	Ch44Model);


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

void SampleApplication::InverseKinematicsScene::UnLoad()
{
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	// IK

	graphicsAssetCreator.DestroyModel("IKModel");
	graphicsAssetCreator.DestroyMesh("targetObjectMesh");
	//graphicsAssetCreator.DestroyAnimation("BD19Animation");
	//graphicsAssetCreator.DestroyAnimation("BDEAnimation");



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

	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
	assetManager->ClearAllAssetRepositories();
}

void SampleApplication::InverseKinematicsScene::Init()
{
	IHCEngine::Core::Time::LockFrameRate(144);

	IHCEngine::Core::GameObject& camera = AddGameObject("camera");
	camera.AddComponent<InverseKinematicsViewer>();
	camera.AddComponent<IHCEngine::Component::ImguiContext_InverseKinematicsViewer>();
	camera.AddComponent<SampleApplication::CameraController>();
	
	//////////////////////////////////////////////////////////////////
	// GameObjects creation and component adding here
	//////////////////////////////////////////////////////////////////

	auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
	IHCEngine::Component::MeshComponent* meshcomponent = nullptr;
	IHCEngine::Component::TextureComponent* texturecomponent = nullptr;
	IHCEngine::Component::PipelineComponent* pipelinecomponent = nullptr;
	IHCEngine::Component::ModelComponent* modelcomponent = nullptr;
	IHCEngine::Component::AnimatorComponent* animatorcomponent = nullptr;

	////// Skeletal Animation ////
	//// Ch44
	IHCEngine::Core::GameObject& IKGobj = AddGameObject("IKGobj");
	IKGobj.transform.SetScale(glm::vec3(0.5, 0.5, 0.5));
	pipelinecomponent = IKGobj.AddComponent<IHCEngine::Component::PipelineComponent>();
	pipelinecomponent->SetPipelineType(IHCEngine::Component::PipelineType::SKELETAL);
	modelcomponent = IKGobj.AddComponent<IHCEngine::Component::ModelComponent>();
	modelcomponent->SetModel(assetManager->GetModelRepository().GetAsset("IKModel"));
	//animatorcomponent = ch44Gobj.AddComponent<IHCEngine::Component::AnimatorComponent>();
	//auto ani1 = assetManager->GetAnimationRepository().GetAsset("CrouchAnimation");
	//auto ani2 = assetManager->GetAnimationRepository().GetAsset("JumpAttackAnimation");

	//animationViewer->AddAnimationGobjs(&ch44Gobj);
	//animationViewer->AddAnimationSlots1(ani1);
	//animationViewer->AddAnimationSlots2(ani2);
	IHCEngine::Core::GameObject& targetGobj = AddGameObject("targetGobj");
	targetGobj.AddComponent<IHCEngine::Component::PipelineComponent>();
	targetGobj.transform.SetScale(glm::vec3(1, 1, 1));
	meshcomponent = targetGobj.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("targetObjectMesh"));
	texturecomponent = targetGobj.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("plainTexture"));


	///////////////////////////
	// Others
	IHCEngine::Core::GameObject& room = AddGameObject("room");
	room.AddComponent<IHCEngine::Component::PipelineComponent>();
	meshcomponent = room.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("roomModel"));
	texturecomponent = room.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("roomTexture"));
	room.transform.SetPosition(glm::vec3(10.0f, 0.01f, 2.0f));
	room.transform.SetRotation(glm::vec3(-90, -90, 0));
	room.transform.SetScale(glm::vec3(1.5, 1.5, 1.5));

	IHCEngine::Core::GameObject& x_axis = AddGameObject("x_axis");
	x_axis.AddComponent<IHCEngine::Component::PipelineComponent>();
	meshcomponent = x_axis.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("x_axisModel"));
	texturecomponent = x_axis.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("plainTexture"));
	x_axis.transform.SetPosition(glm::vec3(1, 0, 0));

	IHCEngine::Core::GameObject& y_axis = AddGameObject("y_axis");
	y_axis.AddComponent<IHCEngine::Component::PipelineComponent>();
	meshcomponent = y_axis.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("y_axisModel"));
	texturecomponent = y_axis.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("plainTexture"));
	y_axis.transform.SetRotation(glm::vec3(0, 0, 90));
	y_axis.transform.SetPosition(glm::vec3(0, 1, 0));

	IHCEngine::Core::GameObject& z_axis = AddGameObject("z_axis");
	z_axis.AddComponent<IHCEngine::Component::PipelineComponent>();
	meshcomponent = z_axis.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("z_axisModel"));
	texturecomponent = z_axis.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("plainTexture"));
	z_axis.transform.SetRotation(glm::vec3(0, 90, 0));
	z_axis.transform.SetPosition(glm::vec3(0, 0, 1));

	IHCEngine::Core::GameObject& grid = AddGameObject("grid");
	grid.AddComponent<IHCEngine::Component::PipelineComponent>();
	meshcomponent = grid.AddComponent<IHCEngine::Component::MeshComponent>();
	meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("gridModel"));
	texturecomponent = grid.AddComponent<IHCEngine::Component::TextureComponent>();
	texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("gridTexture"));
	grid.transform.SetPosition(glm::vec3(0, -0.01, 0));
}

void SampleApplication::InverseKinematicsScene::Reset()
{

}

void SampleApplication::InverseKinematicsScene::createTargetObjectMesh()
{
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();
	IHCEngine::Graphics::IHCMesh::Builder targetObjectBuilder;
	targetObjectBuilder.vertices =
	{
		{ {-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f} , {0.0f, 0.0f, 1.0f}}
	};
	targetObjectBuilder.indices =
	{
		0, 1, 2,    1, 3, 2,  // Left face
		4, 6, 5,    5, 6, 7,  // Right face
		0, 2, 4,    2, 6, 4,  // Bottom face
		1, 5, 3,    3, 5, 7,  // Top face
		0, 4, 1,    1, 4, 5,  // Front face
		2, 3, 6,    3, 7, 6   // Back face
	};
	graphicsAssetCreator.CreateMesh("targetObjectMesh", targetObjectBuilder);
}

void SampleApplication::InverseKinematicsScene::createGridMeshAndLoadGridTexture()
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
void SampleApplication::InverseKinematicsScene::createAxisMeshAndLoadAxisTexture()
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
