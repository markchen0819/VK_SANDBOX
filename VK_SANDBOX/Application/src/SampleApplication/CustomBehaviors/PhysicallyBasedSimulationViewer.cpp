#include "PhysicallyBasedSimulationViewer.h"

#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"

namespace SampleApplication
{
	PhysicallyBasedSimulationViewer::PhysicallyBasedSimulationViewer()
	{
	}

	PhysicallyBasedSimulationViewer::~PhysicallyBasedSimulationViewer()
	{
	}

	void PhysicallyBasedSimulationViewer::Awake()
	{
		auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();

		// AnimationViewerInput
		auto clothGobj = sceneManager->GetActiveScene()->GetGameObjectByName("cloth");
		auto meshComponent = clothGobj->GetComponent<IHCEngine::Component::MeshComponent>();

		physicsSolver.SetTargetMesh(meshComponent);
		physicsSolver.SetupParticles();
		physicsSolver.SetPinnedParticles(99, true);
		physicsSolver.SetPinnedParticles(90, true);
		//physicsSolver.SetPinnedParticles(8, true);
		//physicsSolver.SetPinnedParticles(6, true);
		int dimension = 10;
		//physicsSolver.SetupSprings(dimension, 10,3,20, 0.9); //euler
		physicsSolver.SetupSprings(dimension, 30, 15, 30, 0.5); // verlet
	}

	void PhysicallyBasedSimulationViewer::Start()
	{

	}

	void PhysicallyBasedSimulationViewer::Update()
	{
		physicsSolver.Update();
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


}
