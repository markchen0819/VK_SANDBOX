#pragma once
#include "Particle.h"

namespace IHCEngine::Physics
{
	class Particle;

	class Spring
	{
	public:

		Spring(const std::string& springType, Particle* particle1, Particle* particle2, float k, float d);

		glm::vec3 GetForceAppliedToParticle(Particle* targetParticle, const glm::vec3& pos, const glm::vec3& vel);

		void ApplyForce(float deltaTime);

	private:

		std::string springType;
		Particle* p1;
		Particle* p2;

		float baseLength;
		float stiffness; // k
		float damping;   // d

		glm::vec3 calculateForce();
	};

}
