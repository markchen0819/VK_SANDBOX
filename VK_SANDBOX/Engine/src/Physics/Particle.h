#pragma once

namespace IHCEngine::Physics
{
	class Spring;

	class Particle
	{
	public:

		Particle(int id, const glm::vec3& position, float mass, const glm::vec3& gravity);

		int GetID() { return id; }
		void SetPinned(bool pinned);
		bool IsPinned() const { return isPinned; }
		void SetPosition(glm::vec3 pos);
		glm::vec3 GetPosition() const { return position; }
		void SetVelocity(glm::vec3 velocity);
		glm::vec3 GetVelocity() const { return velocity; }
		void SetMass(float m) { mass = m; }
		float GetMass() const { return mass; }

		void AddConnectedSpring(Spring* spring);

		void ApplyForce(const glm::vec3& force);

		void EulerIntegrate(float deltaTime);
		void VerletIntegrate(float deltaTime);
		void RungeKutta2Integrate(float deltaTime);
		void RungeKutta4Integrate(float deltaTime);

	private:

		int id = 0;

		bool isPinned = false;
		float mass = 1.0f;

		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 acceleration;
		glm::vec3 force;

		glm::vec3 computeVelocity(float deltaTime);
		glm::vec3 computeAcceleration(float deltaTime, glm::vec3 const& velocity);


		// RK2 and RK4
		glm::vec3 previousPosition; // For Verlet and RK2 midpoint calculation
		glm::vec3 k1Velocity, k2Velocity, k3Velocity, k4Velocity; // For RK4
		glm::vec3 k1Position, k2Position, k3Position, k4Position; // For RK4
		std::vector<Spring*> connectedSprings;
		glm::vec3 calculateForces(const glm::vec3& pos, const glm::vec3& vel);
		glm::vec3 gravity;
	};

}
