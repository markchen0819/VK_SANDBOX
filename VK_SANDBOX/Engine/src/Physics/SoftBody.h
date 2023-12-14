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

		void Update();

		IntegrationMethod GetIntegrationMethod() const { return integrationMethod; }
		void SetIntegrationMethod(IntegrationMethod method);

		// Set in order
		void SetTargetMesh(int width, int height, Component::MeshComponent* meshComponent); // Assume providing a NxN mesh
		void SetupParticles();
		void SetupSprings(float kElastic, float kBend, float kShear, float damping);
		void SetPinnedParticles(int id, bool isPinned);
		glm::vec3 GetCenter();

		// Apply same on every particle inside
		void ApplyForce(const glm::vec3& force);
		void ApplyAcceleration(const glm::vec3& acc);
		void ApplyForceOnFace(const glm::vec3& force);


		// Simple Collision with sphere
		void CheckCollisionFromSphere(glm::vec3 pos, float radius);
		void SetSphere(glm::vec3 pos, float radius);

	private:

		// Cloth
		IntegrationMethod integrationMethod = IntegrationMethod::RungeKutta4;
		Graphics::IHCMesh* mesh;
		int width = 0;
		int height = 0;

		std::vector<Particle> particles;
		std::vector<std::unique_ptr<Spring>> springs;

		// Sphere
		glm::vec3 sphereCenterPosition;
		float sphereRadius;
	};
}

