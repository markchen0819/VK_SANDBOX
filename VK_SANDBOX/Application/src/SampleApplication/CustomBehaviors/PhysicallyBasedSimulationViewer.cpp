#include "PhysicallyBasedSimulationViewer.h"

#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Input/Input.h"
#include "../../../../Engine/src/Core/Time/Time.h"

namespace SampleApplication
{
	void PhysicallyBasedSimulationViewer::Awake()
	{
		auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();

		// Cloth setup
		auto clothGobj = sceneManager->GetActiveScene()->GetGameObjectByName("cloth");
		auto meshComponent = clothGobj->GetComponent<IHCEngine::Component::MeshComponent>();

		cloth.SetTargetMesh(35, 35, meshComponent);
		cloth.SetupParticles();
		cloth.SetPinnedParticles(1224, true);
		cloth.SetPinnedParticles(1190, true);
		cloth.SetupSprings(10, 5, 10,0.9);

		sphereGobj = sceneManager->GetActiveScene()->GetGameObjectByName("sphere");

	}

	void PhysicallyBasedSimulationViewer::Start()
	{

	}

	void PhysicallyBasedSimulationViewer::Update()
	{
		float dt = IHCEngine::Core::Time::GetDeltaTime();
		accumulatedTime += dt;
		if (startIntegrateTime > accumulatedTime) return;


		sphereControl();


		cloth.SetSphere(sphereGobj->transform.GetPosition(), 1.2);

		cloth.Update();

		if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_SPACE))
		{
			cloth.SetPinnedParticles(1224, false);
		}
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
