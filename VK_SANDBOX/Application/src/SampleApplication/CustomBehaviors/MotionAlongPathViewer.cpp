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
		auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
		auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
		// Motion blending
		auto walkAnimation = assetManager->GetAnimationRepository().GetAsset("WalkAnimation");
		auto runAnimation = assetManager->GetAnimationRepository().GetAsset("RunAnimation");
		blendTree.SetLowerBoundSpeed(0.06);
		blendTree.SetUpperBoundSpeed(0.14);
		blendTree.SetAnimationA(walkAnimation);
		blendTree.SetAnimationB(runAnimation);

		// TestMoveGobj
		testMoveGobj = sceneManager->GetActiveScene()->GetGameObjectByName("Ch44Gobj1");
		animator = testMoveGobj->GetComponent<IHCEngine::Component::AnimatorComponent>();

		animator->SetAnimationType(IHCEngine::Graphics::AnimationType::BLEND_TREE);
		animator->SetBlendTree(&blendTree);


		//testMoveGobj2 = sceneManager->GetActiveScene()->GetGameObjectByName("Ch44Gobj2");
		//animator2 = testMoveGobj2->GetComponent<IHCEngine::Component::AnimatorComponent>();


		testMoveGobj->transform.SetScale(glm::vec3(0.04, 0.04, 0.04));
		//testMoveGobj2->transform.SetScale(glm::vec3(0.04, 0.04, 0.04));

		changeNextDataSet();
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
			//animator2->PlayAnimation();
		}

		if (isMoving)
		{
			float dt = IHCEngine::Core::Time::GetDeltaTime();

			// using Time-Distance function to get position on Space Curve
			passedTime += dt;
			float passedDistance = speedControl.GetDistance(passedTime);
			glm::vec3 moveToPos = spaceCurve.GetPositionOnCurve(passedDistance);
			currentSpeed = (passedDistance - prevFrameDistance) / dt;
			prevFrameDistance = passedDistance;



			testMoveGobj->transform.SetPosition(moveToPos);
			//testMoveGobj2->transform.SetPosition(targetPos);


			float n = paceControl.GetAnimatorSpeedModifier("WalkAnimation", currentSpeed, 1.0, totalTime);
			float n2 = paceControl.GetAnimatorSpeedModifier("RunAnimation", currentSpeed, 1.0, totalTime);

			animator->SetSpeed(n); // This is animation cycle speed
			blendTree.SetCurrentSpeed(currentSpeed); // This is movement speed
			//animator2->SetSpeed(n2);


			// Orientation Control
			glm::vec3 rotation = orientationControl.GetRotation(spaceCurve, passedDistance);
			testMoveGobj->transform.SetRotation(rotation);


			if(passedTime >= totalTime)
			{
				currentSpeed = 0;
				isMoving = false;
				animator->StopAnimation();
				//animator2->StopAnimation();
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

			totalTime = 8;
			speedControl.SetTimings(2.0, 6.0, 8.0); //ease in out
		}
		else if (dataSetIndex==1) // Curvy path with up downs
		{		
			data.push_back(glm::vec3(0, 0.5, 5));
			data.push_back(glm::vec3(10, 0.5, 10));
			data.push_back(glm::vec3(7, 0.5, 0));
			data.push_back(glm::vec3(8, 0.5, -13));
			data.push_back(glm::vec3(-7, 0.5,-20 ));
			data.push_back(glm::vec3(-15, 0.5, 0));
			data.push_back(glm::vec3(-7, 0.5, 7));
			data.push_back(glm::vec3(-5, 0.5, -5));
			data.push_back(glm::vec3(0, 1, -5));
			data.push_back(glm::vec3(-5, 1, 15));
			data.push_back(glm::vec3(0, 0.5, 10));

			totalTime = 20;
			speedControl.SetTimings(5.0, 15.0, totalTime);
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

			totalTime = 8;
			speedControl.SetTimings(2.0, 6.0, 8.0); //ease in out
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
		//testMoveGobj2->transform.SetPosition(data[0]);
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
