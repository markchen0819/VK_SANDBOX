#pragma once
#include "Particle.h"

namespace IHCEngine::Physics
{
	class Particle;

	enum class SpringType
	{
		ELASTIC, // one next to
		BEND,    // two next to
		SHEAR,   // diagonal
		DEFAULT
	};

	class Spring
	{
	public:

		Spring(const SpringType& springType, Particle* particle1, Particle* particle2, float stiffness, float damping);

		glm::vec3 GetForceAppliedToParticle(Particle* targetParticle, const glm::vec3& pos, const glm::vec3& vel);

		SpringType& GetSpringType() { return springType; }

		void ApplyForce(float deltaTime);

		void ApplyStretchConstraint();

	private:

		SpringType springType = SpringType::DEFAULT;
		Particle* p1 = nullptr;
		Particle* p2 = nullptr;

		float baseLength;
		float stiffness; // k
		float damping;   // d

		glm::vec3 calculateForce();
	};

}
