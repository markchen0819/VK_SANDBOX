#pragma once

namespace IHCEngine::Physics
{
	class Particle
	{
	public:

		Particle(int id, const glm::vec3& position, float mass);

		int GetID() { return id; }
		void SetPinned(bool p) { isPinned = p; }
		bool IsPinned() const { return isPinned; }
		void SetPosition(glm::vec3 pos) { position = pos; }
		glm::vec3 GetPosition() const { return position; }
		void SetVelocity(glm::vec3 vel) { velocity = vel; }
		glm::vec3 GetVelocity() const { return velocity; }
		void SetMass(float m) { mass = m; }
		float GetMass() const { return mass; }

		// Force
		void ApplyForce(const glm::vec3& force);

		// Calculate acceleration/ velocity/ position
		void EulerIntegrate(float deltaTime);
		void VerletIntegrate(float deltaTime);
		void RungeKutta2Integrate(float deltaTime);
		void RungeKutta4Integrate(float deltaTime);

	private:

		int id = 0;
		float mass = 1.0f;

		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 acceleration;
		glm::vec3 force;

		bool isPinned = false;

		glm::vec3 computeVelocity(float deltaTime);
		glm::vec3 computeAcceleration(float deltaTime, glm::vec3 const& velocity);
	};

}
