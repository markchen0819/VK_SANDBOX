#include "MotionAlongPathViewer.h"

#include <iomanip>

#include "../../../../Engine/src/Core/Scene/GameObject.h"
#include "../../../../Engine/src/Core/Scene/Components/LineRendererComponent.h"
#include "../../../../Engine/src/Core/Scene/Scene.h"
#include "../../../../Engine/src/Core/Scene/Components/MeshComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/TextureComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/PipelineComponent.h"
#include "../../../../Engine/src/Core/Locator/AssetManagerLocator.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Core/Scene/Components/AnimatorComponent.h"
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
		// TestMoveGobj
		auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
		auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();

		testMoveGobj = sceneManager->GetActiveScene()->GetGameObjectByName("Ch44Model");

		animator = testMoveGobj->GetComponent<IHCEngine::Component::AnimatorComponent>();

		//testMoveGobj = &sceneManager->GetActiveScene()->AddGameObject("TestMoveGobj");
		//testMoveGobj->AddComponent<IHCEngine::Component::PipelineComponent>();
		//auto meshcomponent = testMoveGobj->AddComponent<IHCEngine::Component::MeshComponent>();
		//meshcomponent->SetMesh(assetManager->GetMeshRepository().GetAsset("controlPointModel"));
		//auto texturecomponent = testMoveGobj->AddComponent<IHCEngine::Component::TextureComponent>();
		//texturecomponent->SetTexture(assetManager->GetTextureRepository().GetAsset("plainTexture"));
		testMoveGobj->transform.SetScale(glm::vec3(0.04, 0.04, 0.04));

		changeNextDataSet();

		totalTime = 8;
		speedControl.SetTimings(2.0, 6.0, 8.0); //ease in out

	}

	void MotionAlongPathViewer::Start()
	{

	}

	void MotionAlongPathViewer::Update()
	{

		if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_RIGHT))
		{
			isMoving = false;
			passedTime = 0;
			prevFrameDistance = 0;
			changeNextDataSet();
		}

		if (IHCEngine::Core::Input::IsKeyUp(GLFW_KEY_P))
		{
			isMoving = true;
			passedTime = 0;
			prevFrameDistance = 0;
			animator->PlayAnimation();
		}

		if (isMoving)
		{
			float dt = IHCEngine::Core::Time::GetDeltaTime();
			passedTime += dt;

			float passedDistance = speedControl.GetDistance(passedTime);
			glm::vec3 targetPos = spaceCurve.GetPositionOnCurve(passedDistance);
			testMoveGobj->transform.SetPosition(targetPos);

			currentSpeed = (passedDistance - prevFrameDistance) / dt;
			prevFrameDistance = passedDistance;

			float cycleDuration = 43.0f / 60.0;
			float normalSpeed = 0.125; // 1.0d / 8s
			float idealPace = normalSpeed * cycleDuration;
			float currentPace = currentSpeed * cycleDuration;

			// Now, calculate the ratio of currentPace to idealPace. This will be your animation speed adjustment.
			float n = currentPace / idealPace;

			//float Pace = strideLength;
			//float n = currentSpeed / Pace;
			animator->SetSpeed(n);


			//currentSpeed*=100; // display only

			if(passedTime >= totalTime)
			{
				currentSpeed = 0;
				isMoving = false;
				animator->StopAnimation();
			}
		}



	}

	void MotionAlongPathViewer::FixedUpdate(){}

	void MotionAlongPathViewer::OnEnable(){}

	void MotionAlongPathViewer::OnDisable(){}

	ArcLengthTable& MotionAlongPathViewer::GetArcLengthTable()
	{
		return spaceCurve.GetArcLengthTable();
	}

	void MotionAlongPathViewer::changeNextDataSet()
	{
		// Set control points
		data.clear();
		if(dataSetIndex==0) // Circular
		{
			data.push_back(glm::vec3(0, 1, 10));
			data.push_back(glm::vec3(10, 1, 0));
			data.push_back(glm::vec3(0, 1, -10));
			data.push_back(glm::vec3(-10, 1, 0));
			data.push_back(glm::vec3(0, 1, 10));
		}
		else if (dataSetIndex==1) // TEST
		{		
			data.push_back(glm::vec3(0, 1, 0));
			data.push_back(glm::vec3(4, 1, -4));
			data.push_back(glm::vec3(8, 1, 0));
			data.push_back(glm::vec3(12, 1, -4));
			data.push_back(glm::vec3(16, 1, 0));
			data.push_back(glm::vec3(20, 1, -4));
			data.push_back(glm::vec3(24, 1, 0));
			data.push_back(glm::vec3(28, 1, -4));
			data.push_back(glm::vec3(32, 1, 0));
			data.push_back(glm::vec3(36, 1, -4));
		}
		else if(dataSetIndex==2) // Detailed Circular
		{
			data.push_back(glm::vec3(0, 1, 10));
			data.push_back(glm::vec3(7.071, 1, 7.071));
			data.push_back(glm::vec3(10, 1, 0));
			data.push_back(glm::vec3(7.071, 1, -7.071));
			data.push_back(glm::vec3(0, 1, -10));
			data.push_back(glm::vec3(-7.071, 1, -7.071));
			data.push_back(glm::vec3(-10, 1, 0));
			data.push_back(glm::vec3(-7.071, 1, 7.071));
			data.push_back(glm::vec3(0, 1, 10));
		}

		// Calculate for motion access
		spaceCurve.SetControlPoints(data);

		// Create Curve visual
		lineRenderer = gameObject->GetComponent<IHCEngine::Component::LineRendererComponent>();
		lineRenderer->SetPoints(spaceCurve.GetPointsForRendering());

		// Create Debugging visual
		createDebugControlPoints();

		// Put moving gobj to start
		testMoveGobj->transform.SetPosition(data[0]);
		// Next set
		dataSetIndex = (dataSetIndex + 1) % 3;
	}

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
			debugControlPoints.push_back(&point);
		}
	}
}
