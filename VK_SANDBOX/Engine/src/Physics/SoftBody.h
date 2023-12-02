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

	class SoftBody
	{
	public:

		SoftBody() = default;
		~SoftBody() = default;

		// Set in order
		void SetTargetMesh(int width, int height, Component::MeshComponent* meshComponent); // Assume providing a NxN mesh
		void SetupParticles();
		void SetupSprings(float kElastic, float kBend, float kShear, float damping);
		void SetPinnedParticles(int id, bool isPinned);

		//
		void SetSphere(glm::vec3 pos, float radius);
		void ApplyForce(const glm::vec3& force);
		void Update();

	private:

		// Cloth
		IntegrationMethod integrationMethod = IntegrationMethod::RungeKutta4;// Verlet;// RungeKutta4;// RungeKutta2;// Verlet;
		Graphics::IHCMesh* mesh;
		int width = 0;
		int height = 0;

		std::vector<Particle> particles;
		std::vector<std::unique_ptr<Spring>> springs;
		glm::vec3 gravity;



		// Sphere
		glm::vec3 sphereCenterPosition;
		float sphereRadius;
		void fixPositionFromCollision(Particle& particle);
	};
}

