#pragma once
#include "Particle.h"
#include "Spring.h"

#include "../Core/Scene/Components/MeshComponent.h"

namespace IHCEngine::Graphics
{
	class IHCMesh;
}

namespace IHCEngine::Physics
{
	enum class IntegrationMethod
	{
		Euler,
		Verlet,
		RungeKutta2,
		RungeKutta4
	};

	class PhysicsSolver
	{
	public:
		PhysicsSolver();
		~PhysicsSolver();

		void SetTargetMesh(Component::MeshComponent* meshComponent);
		void SetupParticles();
		void SetupSprings(int dimension, float kElastic, float kBend, float kShear, float damping);
		void SetPinnedParticles(int id, bool isPinned);

		void Update();

	private:

		IntegrationMethod integrationMethod = IntegrationMethod::Verlet;
		Graphics::IHCMesh* mesh;
		std::vector<Particle> particles;
		std::vector<Spring> springs;
		glm::vec3 gravity;
	};
}

