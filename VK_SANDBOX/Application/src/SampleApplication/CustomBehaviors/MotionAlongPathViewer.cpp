#include "MotionAlongPathViewer.h"

#include "../../../../Engine/src/Core/Scene/GameObject.h"
#include "../../../../Engine/src/Core/Scene/Components/LineRendererComponent.h"
#include "../../../../Engine/src/Core/Scene/Scene.h"
#include "../../../../Engine/src/Core/Scene/Components/MeshComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/TextureComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/PipelineComponent.h"
#include "../../../../Engine/src/Core/Locator/AssetManagerLocator.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"

namespace IHCEngine::Component
{
	class PipelineComponent;
}

namespace SampleApplication
{
	MotionAlongPathViewer::MotionAlongPathViewer()
	{}

	MotionAlongPathViewer::~MotionAlongPathViewer()
	{
		for (auto pt : debugControlPoints)
		{
			pt->Destroy();
		}
		debugControlPoints.clear();
	}

	void MotionAlongPathViewer::Awake()
	{


		// TEST
		//p.push_back(glm::vec3(0, 1, 0));
		//p.push_back(glm::vec3(4, 1, -4)); 
		//p.push_back(glm::vec3(8, 1, 0));
		//p.push_back(glm::vec3(12, 1, -4));

		// Circular
		data.push_back(glm::vec3(0, 1, 10));
		data.push_back(glm::vec3(10, 1, 0));
		data.push_back(glm::vec3(0, 1, -10));
		data.push_back(glm::vec3(-10, 1, 0));
		data.push_back(glm::vec3(0, 1, 10));

		// Detailed Circular
		//data.push_back(glm::vec3(0, 1, 10));
		//data.push_back(glm::vec3(7.071, 1, 7.071));
		//data.push_back(glm::vec3(10, 1, 0));
		//data.push_back(glm::vec3(7.071, 1, -7.071));
		//data.push_back(glm::vec3(0, 1, -10));
		//data.push_back(glm::vec3(-7.071, 1, -7.071));
		//data.push_back(glm::vec3(-10, 1, 0));
		//data.push_back(glm::vec3(-7.071, 1, 7.071));
		//data.push_back(glm::vec3(0, 1, 10));

		spaceCurve.SetControlPoints(data);


		lineRenderer = gameObject->GetComponent<IHCEngine::Component::LineRendererComponent>();
		lineRenderer->SetPoints(spaceCurve.GetPoints());

		createDebugControlPoints();

	}

	void MotionAlongPathViewer::Start()
	{

	}

	void MotionAlongPathViewer::Update()
	{

	}

	void MotionAlongPathViewer::FixedUpdate(){}

	void MotionAlongPathViewer::OnEnable(){}

	void MotionAlongPathViewer::OnDisable(){}

	void MotionAlongPathViewer::createDebugControlPoints()
	{
		for(auto pt : debugControlPoints)
		{
			pt->Destroy();
		}
		debugControlPoints.clear();
		// Others
		auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
		auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
		for(int i=0; i< data.size(); ++i)
		{
			std::string id = "debugControlPoint_" + std::to_string(controlPointID);
			IHCEngine::Core::GameObject& point = sceneManager->GetActiveScene()->AddGameObject(id);
			point.AddComponent<IHCEngine::Component::PipelineComponent>();
			auto meshcomponent = point.AddComponent<IHCEngine::Component::MeshComponent>();
			meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("controlPointModel"));
			auto texturecomponent = point.AddComponent<IHCEngine::Component::TextureComponent>();
			texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("plainTexture"));
			point.transform.SetPosition(data[i]);
			point.transform.SetScale(glm::vec3(0.2,0.2,0.2));
			controlPointID++;
		}

	}
}
