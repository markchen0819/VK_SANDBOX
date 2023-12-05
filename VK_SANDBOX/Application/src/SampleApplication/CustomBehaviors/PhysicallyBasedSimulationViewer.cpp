#include "PhysicallyBasedSimulationViewer.h"

#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Input/Input.h"
#include "../../../../Engine/src/Core/Time/Time.h"

namespace SampleApplication
{
	void PhysicallyBasedSimulationViewer::Awake()
	{
		auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();



		gravityAcceleration = glm::vec3(0.0f, -9.81, 0.0f);
		windForce = glm::vec3(0, 1, 1);

		// Cloth setup
		auto clothGobj = sceneManager->GetActiveScene()->GetGameObjectByName("cloth");
		auto meshComponent = clothGobj->GetComponent<IHCEngine::Component::MeshComponent>();

		cloth.SetTargetMesh(35, 35, meshComponent);
		cloth.SetupParticles();

		cloth.SetPinnedParticles(0, true);
		cloth.SetPinnedParticles(34, true);
		cloth.SetPinnedParticles(1224, true);
		cloth.SetPinnedParticles(1190, true);

		cloth.SetupSprings(10, 5, 10,0.9);

		sphereGobj = sceneManager->GetActiveScene()->GetGameObjectByName("sphere");
		sphereGobj->transform.SetPosition(glm::vec3(0, 7, 0));
	}

	void PhysicallyBasedSimulationViewer::Start()
	{

	}

	void PhysicallyBasedSimulationViewer::Update()
	{
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

	void PhysicallyBasedSimulationViewer::FixedUpdate()
	{

	}

	void PhysicallyBasedSimulationViewer::OnEnable()
	{

	}

	void PhysicallyBasedSimulationViewer::OnDisable()
	{

	}

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
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_UP))
		{
			auto p = -1.0f * forward * movementSpeed * dt;
			sphereGobj->transform.Translate(p);
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_DOWN))//
		{
			auto p = forward * movementSpeed * dt;
			sphereGobj->transform.Translate(p);
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_LEFT))//
		{
			auto p = -1.0f * right * movementSpeed * dt;
			sphereGobj->transform.Translate(p);
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_RIGHT))//
		{
			auto p = right * movementSpeed * dt;
			sphereGobj->transform.Translate(p);
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_E))
		{
			auto p = up * movementSpeed * dt;
			sphereGobj->transform.Translate(p);
		}
		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_Q))
		{
			auto p = -1.0f * up * movementSpeed * dt;
			sphereGobj->transform.Translate(p);
		}
	}
}
