#include "MotionAlongPathViewer.h"

#include <iomanip>

#include "../../../../Engine/src/Core/Scene/GameObject.h"
#include "../../../../Engine/src/Core/Scene/Components/LineRendererComponent.h"
#include "../../../../Engine/src/Core/Scene/Scene.h"
#include "../../../../Engine/src/Core/Scene/Components/MeshComponent.h"
#include "../../../../Engine/src/Core/Scene/Components/TextureComponent.h"
#include "../../../../Engine/src/Core/Locator/AssetManagerLocator.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Core/Scene/Components/AnimatorComponent.h"
#include "../../../../Engine/src/Input/Input.h"
#include "../../../../Engine/src/Core/Time/Time.h"
#include "../../../../Engine/src/Core/Locator/AppWindowLocator.h"
#include "../../../../Engine/src/Graphics/RenderSystems/RenderSystem.h"

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
		// Animation Viewer Input
		window = IHCEngine::Core::AppWindowLocator::GetAppWindow()->GetWindowHandle();
		camera = &(this->gameObject->GetScene()->GetCamera());
		camera->transform.Rotate(glm::vec3(-20, 0, 0));
		camera->transform.SetPosition(glm::vec3(0.0f, 11.0f, 30.0f));
		glm::vec3 cameraPosWithoutY = camera->transform.GetPosition();
		angleRespectToCenterPoint = 90;
		cameraPosWithoutY.y = 0;
		distanceToCenterPoint = length(centerPoint - cameraPosWithoutY);
		camera->LookAt(glm::vec3(0, 5, 0));

		// Motion blending
		auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
		auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
		auto walkAnimation = assetManager->GetAnimationRepository().GetAsset("WalkAnimation");
		auto runAnimation = assetManager->GetAnimationRepository().GetAsset("RunAnimation");
		blendTree.SetAnimationA(walkAnimation);
		blendTree.SetAnimationB(runAnimation);
		upperBoundSpeed = 0.14f;
		lowerBoundSpeed = 0.06f;
		blendTree.SetUpperBoundSpeed(upperBoundSpeed);
		blendTree.SetLowerBoundSpeed(lowerBoundSpeed);
		movingGobj = sceneManager->GetActiveScene()->GetGameObjectByName("Ch44Gobj1");
		movingGobj->transform.SetScale(glm::vec3(0.04, 0.04, 0.04));
		animator = movingGobj->GetComponent<IHCEngine::Component::AnimatorComponent>();
		animator->SetAnimationType(IHCEngine::Graphics::AnimationType::BLEND_TREE);
		animator->SetBlendTree(&blendTree);

		changeNextDataSet();
	}

	void MotionAlongPathViewer::Start(){}

	void MotionAlongPathViewer::Update()
	{
		if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_RIGHT))
		{
			isMoving = false;
			passedTime = 0;
			prevFrameDistance = 0;
			animator->StopAnimation();
			changeNextDataSet();
		}
		if (IHCEngine::Core::Input::IsKeyUp(GLFW_KEY_SPACE))
		{
			isMoving = true;
			passedTime = 0;
			prevFrameDistance = 0;
			animator->PlayAnimation();
		}

		if (isMoving)
		{
			float dt = IHCEngine::Core::Time::GetDeltaTime();

			// Using Time-Distance function to get position on Space Curve
			passedTime += dt;
			float passedDistance = speedControl.GetDistance(passedTime);
			glm::vec3 moveToPos = spaceCurve.GetPositionOnCurve(passedDistance);
			currentSpeed = (passedDistance - prevFrameDistance) / dt;
			prevFrameDistance = passedDistance;
			movingGobj->transform.SetPosition(moveToPos);

			// Set pace for animation (Assume Walk & Run has the same pace)
			float n = paceControl.GetAnimatorSpeedModifier("WalkAnimation", currentSpeed, 1.0, totalTime);
			//float n2 = paceControl.GetAnimatorSpeedModifier("RunAnimation", currentSpeed, 1.0, totalTime);
			animator->SetSpeed(n); // This is animation cycle speed

			// Set animation blending based on movement Speed
			blendTree.SetCurrentSpeed(currentSpeed); // This is movement speed
			blendRatio = blendTree.GetBlendFactor();

			// Orientation Control
			glm::vec3 rotation = orientationControl.GetRotation(spaceCurve, passedDistance);
			movingGobj->transform.SetRotation(rotation);


			if(passedTime >= totalTime)
			{
				currentSpeed = 0;
				isMoving = false;
				animator->StopAnimation();
			}
		}
		HandleAnimationViewerInput();
		HandleOtherInputs();
	}

	void MotionAlongPathViewer::FixedUpdate(){}

	void MotionAlongPathViewer::OnEnable(){}

	void MotionAlongPathViewer::OnDisable(){}

	void MotionAlongPathViewer::HandleAnimationViewerInput()
	{
		float dt = IHCEngine::Core::Time::GetDeltaTime();
		auto cameraUp = camera->transform.GetUp(); //camera->GetUp();
		auto cameraRight = camera->transform.GetRight(); //camera->GetRight();
		auto cameraForward = camera->transform.GetForward();// camera->GetForward();

		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_0)) // rotate right around target
		{
			angleRespectToCenterPoint += angleSpeed * dt;
			glm::vec3 cameraPos = camera->transform.GetPosition();
			glm::vec3 newPos;
			newPos.x = centerPoint.x + distanceToCenterPoint * cos(glm::radians(angleRespectToCenterPoint));
			newPos.y = cameraPos.y;  // Y remains the same as we're not moving vertically
			newPos.z = centerPoint.z + distanceToCenterPoint * sin(glm::radians(angleRespectToCenterPoint));
			camera->transform.SetWorldPosition(newPos);
			camera->LookAt(glm::vec3(0, 5, 0));
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_DECIMAL))// rotate left around target
		{
			angleRespectToCenterPoint -= angleSpeed * dt;
			glm::vec3 cameraPos = camera->transform.GetPosition();
			glm::vec3 newPos;
			newPos.x = centerPoint.x + distanceToCenterPoint * cos(glm::radians(angleRespectToCenterPoint));
			newPos.y = cameraPos.y;  // Y remains the same as we're not moving vertically
			newPos.z = centerPoint.z + distanceToCenterPoint * sin(glm::radians(angleRespectToCenterPoint));
			camera->transform.SetWorldPosition(newPos);
			camera->LookAt(glm::vec3(0, 5, 0));
		}
		if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_KP_5)) // reset
		{
			angleRespectToCenterPoint = 90;
			glm::vec3 cameraPos = camera->transform.GetPosition();
			glm::vec3 newPos;
			newPos.x = centerPoint.x + distanceToCenterPoint * cos(glm::radians(angleRespectToCenterPoint));
			newPos.y = cameraPos.y;  // Y remains the same as we're not moving vertically
			newPos.z = centerPoint.z + distanceToCenterPoint * sin(glm::radians(angleRespectToCenterPoint));
			camera->transform.SetWorldPosition(newPos);
			camera->LookAt(glm::vec3(0, 5, 0));
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_4)) // left
		{
			auto p = -cameraRight * movementSpeed * dt;
			camera->transform.Translate(p);
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_6)) // right
		{
			auto p = cameraRight * movementSpeed * dt;
			camera->transform.Translate(p);
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_8)) // up
		{
			auto p = cameraUp * movementSpeed * dt;
			camera->transform.Translate(p);
			auto cameraX = camera->transform.GetPosition().x;
			camera->LookAt(glm::vec3(cameraX, 5, 0));
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_2)) // down
		{
			auto p = -cameraUp * movementSpeed * dt;
			camera->transform.Translate(p);
			auto cameraX = camera->transform.GetPosition().x;
			camera->LookAt(glm::vec3(cameraX, 5, 0));
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_ADD)) // zoom in
		{
			float newFov = camera->GetFOV() - 1 * zoomSpeed;
			camera->SetFOV(newFov);
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_SUBTRACT)) // zoom in
		{
			float newFov = camera->GetFOV() + 1 * zoomSpeed;
			camera->SetFOV(newFov);
		}
	}

	void MotionAlongPathViewer::HandleOtherInputs()
	{
		if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_B))
		{
			// bone drawing on/off.
			IHCEngine::Graphics::RenderSystem::debugBonesEnabled = !IHCEngine::Graphics::RenderSystem::debugBonesEnabled;
		}
		if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_M))
		{
			// mesh drawing on/off.
			IHCEngine::Graphics::RenderSystem::animationMeshEnabled = !IHCEngine::Graphics::RenderSystem::animationMeshEnabled;
		}
	}

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

			easeInTiming = 2.0f;
			easeOutTiming = 6.0f;
			totalTime = 8.0f;
			upperBoundSpeed = 0.14f;
			lowerBoundSpeed = 0.06f;
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

			easeInTiming = 10.0f;
			easeOutTiming = 13.0f;
			totalTime = 20.0f;
			upperBoundSpeed = 0.071f;
			lowerBoundSpeed = 0.04f;
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

			easeInTiming = 1.5f;
			easeOutTiming = 4.5f;
			totalTime = 6.0f;
			upperBoundSpeed = 0.20f;
			lowerBoundSpeed = 0.08f;
		}

		speedControl.SetTimings(easeInTiming, easeOutTiming, totalTime); //ease in out
		blendTree.SetUpperBoundSpeed(upperBoundSpeed);
		blendTree.SetLowerBoundSpeed(lowerBoundSpeed);

		// Calculate for motion access
		spaceCurve.SetControlPoints(data);

		// Create Curve visual
		lineRenderer = gameObject->GetComponent<IHCEngine::Component::LineRendererComponent>();
		lineRenderer->SetPoints(spaceCurve.GetPointsForRendering());

		// Create Debugging visual
		createDebugControlPoints();

		// Put moving gobj to start
		movingGobj->transform.SetPosition(data[0]);

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
