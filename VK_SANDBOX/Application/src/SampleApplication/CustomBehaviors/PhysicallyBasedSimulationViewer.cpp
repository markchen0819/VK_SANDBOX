#include "PhysicallyBasedSimulationViewer.h"

#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Input/Input.h"
#include "../../../../Engine/src/Core/Time/Time.h"
#include "../../../../Engine/src/Graphics/RenderSystems/RenderSystem.h"

namespace SampleApplication
{
	void PhysicallyBasedSimulationViewer::Awake()
	{
		auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();

		// Camera 
		auto cameraGobj = sceneManager->GetActiveScene()->GetGameObjectByName("camera");
		camera = &(this->gameObject->GetScene()->GetCamera());
		camera->transform.Rotate(glm::vec3(-10, 0, 0));
		camera->transform.SetPosition(glm::vec3(0.0f, 13.0f, 7.0f));
		glm::vec3 cameraPosWithoutY = camera->transform.GetPosition();
		angleRespectToCenterPoint = 90;
		cameraPosWithoutY.y = 0;
		distanceToCenterPoint = glm::length(centerPoint - cameraPosWithoutY);
		camera->LookAt(glm::vec3(0, 8, 0));

		// Environment force setup
		gravityAcceleration = glm::vec3(0.0f, -9.81, 0.0f);
		windForce = glm::vec3(0, 1, 1);

		// Cloth 
		auto clothGobj = sceneManager->GetActiveScene()->GetGameObjectByName("cloth");
		auto meshComponent = clothGobj->GetComponent<IHCEngine::Component::MeshComponent>();
		cloth.SetTargetMesh(35, 35, meshComponent);
		cloth.SetupParticles();
		cloth.SetPinnedParticles(0, true);
		cloth.SetPinnedParticles(34, true);
		cloth.SetPinnedParticles(1224, true);
		cloth.SetPinnedParticles(1190, true);
		cloth.SetupSprings(10, 5, 10,0.9);
		// Sphere
		sphereGobj = sceneManager->GetActiveScene()->GetGameObjectByName("sphere");
		sphereGobj->transform.SetPosition(glm::vec3(0, 7, 0));
	}

	void PhysicallyBasedSimulationViewer::Start() {}
	void PhysicallyBasedSimulationViewer::Update()
	{
		HandleAnimationViewerInput();

		// Wait for frame rate steady
		float dt = IHCEngine::Core::Time::GetDeltaTime();
		accumulatedTime += dt;
		if (startIntegrateTime > accumulatedTime) return;

		// Sphere
		sphereControl();

		// Cloth
		cloth.ApplyAcceleration(gravityAcceleration);
		glm::vec3 windDirection = glm::normalize(cloth.GetCenter() - sphereGobj->transform.GetPosition());
		windForce = windDirection * windStrength;
		cloth.ApplyForceOnFace(windForce);
		cloth.SetSphere(sphereGobj->transform.GetPosition(), 1.1);
		cloth.Update();

	}
	void PhysicallyBasedSimulationViewer::FixedUpdate() {}
	void PhysicallyBasedSimulationViewer::OnEnable() {}
	void PhysicallyBasedSimulationViewer::OnDisable() {}


	void PhysicallyBasedSimulationViewer::SetPinned(int pinID)
	{
		if (pinID == 1)
		{
			isP1Pinned = !isP1Pinned;
			cloth.SetPinnedParticles(0, isP1Pinned);
		}
		if (pinID == 2)
		{
			isP2Pinned = !isP2Pinned;
			cloth.SetPinnedParticles(34, isP2Pinned);
		}
		if (pinID == 3)
		{
			isP3Pinned = !isP3Pinned;
			cloth.SetPinnedParticles(1224, isP3Pinned);
		}
		if (pinID == 4)
		{
			isP4Pinned = !isP4Pinned;
			cloth.SetPinnedParticles(1190, isP4Pinned);
		}
	}

	void PhysicallyBasedSimulationViewer::sphereControl()
	{
		float dt = IHCEngine::Core::Time::GetDeltaTime();
		auto up = glm::vec3(0, 1, 0);
		auto right = glm::vec3(1, 0, 0);
		auto forward = glm::vec3(0, 0, 1);

		// Translate
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_W)) //GLFW_KEY_UP))
		{
			auto p = -1.0f * forward * sphereMovementSpeed * dt;
			sphereGobj->transform.Translate(p);
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_S)) //GLFW_KEY_DOWN))//
		{
			auto p = forward * sphereMovementSpeed * dt;
			sphereGobj->transform.Translate(p);
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_A)) //GLFW_KEY_LEFT))//
		{
			auto p = -1.0f * right * sphereMovementSpeed * dt;
			sphereGobj->transform.Translate(p);
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_D)) //GLFW_KEY_RIGHT))//
		{
			auto p = right * sphereMovementSpeed * dt;
			sphereGobj->transform.Translate(p);
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_E))
		{
			auto p = up * sphereMovementSpeed * dt;
			sphereGobj->transform.Translate(p);
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_Q))
		{
			auto p = -1.0f * up * movementSpeed * dt;
			sphereGobj->transform.Translate(p);
		}
	}

	void PhysicallyBasedSimulationViewer::HandleAnimationViewerInput()
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
			camera->LookAt(glm::vec3(0, 8, 0));
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
			camera->LookAt(glm::vec3(0, 8, 0));
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
			camera->LookAt(glm::vec3(0, 8, 0));
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
			camera->LookAt(glm::vec3(cameraX, 8, 0));
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_2)) // down
		{
			auto p = -cameraUp * movementSpeed * dt;
			camera->transform.Translate(p);
			auto cameraX = camera->transform.GetPosition().x;
			camera->LookAt(glm::vec3(cameraX, 8, 0));
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
		if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_B))
		{
			// bone drawing on/off.
			IHCEngine::Graphics::RenderSystem::wireframeEnabled = !IHCEngine::Graphics::RenderSystem::wireframeEnabled;
		}
	}
}
