#include "MotionAlongPathViewer.h"

#include "../../../../Engine/src/Core/Scene/GameObject.h"
#include "../../../../Engine/src/Core/Scene/Components/LineRendererComponent.h"
#include "../../../../Engine/src/Core/Scene/Scene.h"
#include "../../../../Engine/src/Core/Scene/Components/MeshComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/TextureComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/PipelineComponent.h"
#include "../../../../Engine/src/Core/Locator/AssetManagerLocator.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Input/Input.h"
#include "../../../../Engine/src/Core/Time/Time.h"

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
		//data.push_back(glm::vec3(0, 1, 0));
		//data.push_back(glm::vec3(4, 1, -4));
		//data.push_back(glm::vec3(8, 1, 0));
		//data.push_back(glm::vec3(12, 1, -4));

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
		lineRenderer->SetPoints(spaceCurve.GetPointsForRendering());

		createDebugControlPoints();


		// TestMoveGobj
		auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
		auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();

		testMoveGobj = &sceneManager->GetActiveScene()->AddGameObject("TestMoveGobj");
		testMoveGobj->AddComponent<IHCEngine::Component::PipelineComponent>();
		auto meshcomponent = testMoveGobj->AddComponent<IHCEngine::Component::MeshComponent>();
		meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("controlPointModel"));
		auto texturecomponent = testMoveGobj->AddComponent<IHCEngine::Component::TextureComponent>();
		texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("plainTexture"));
		testMoveGobj->transform.SetPosition(data[0]);
		testMoveGobj->transform.SetScale(glm::vec3(0.4, 0.4, 0.4));

		testMoveGobj2 = & sceneManager->GetActiveScene()->AddGameObject("TestMoveGobj2");
		testMoveGobj2->AddComponent<IHCEngine::Component::PipelineComponent>();
		meshcomponent = testMoveGobj2->AddComponent<IHCEngine::Component::MeshComponent>();
		meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("controlPointModel"));
		texturecomponent = testMoveGobj2->AddComponent<IHCEngine::Component::TextureComponent>();
		texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("plainTexture"));
		testMoveGobj2->transform.SetPosition(data[0]);
		testMoveGobj2->transform.SetScale(glm::vec3(0.4, 0.4, 0.4));

		speedControl.SetTimings(2.0, 6.0, 8.0); //ease in out


	}

	void MotionAlongPathViewer::Start()
	{

	}

	void MotionAlongPathViewer::Update()
	{
		if (startMove)
		{
			float dt = IHCEngine::Core::Time::GetDeltaTime();
			passedTime += dt ;

			float distance1 = speedControl.GetDistance(passedTime);
			glm::vec3 targetPos = spaceCurve.GetPositionOnCurve(distance1);
			testMoveGobj->transform.SetPosition(targetPos);

			float distance2 = passedTime / 8; // constant for compare
			if (distance2 > 1.0) distance2 = 1.0;
			glm::vec3 targetPos2 = spaceCurve.GetPositionOnCurve(distance2);
			testMoveGobj2->transform.SetPosition(targetPos2);

			std::cout <<"passedTime "<< passedTime << std::endl;

			std::cout << "distance1 " << distance1 << std::endl;
			std::cout << "distance2 " << distance2 << std::endl;
		

			if(passedTime>=8)
			{
				startMove = false;
			}
		}

		if(IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_P))
		{
			startMove = true;
			passedTime = 0;
		}

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
