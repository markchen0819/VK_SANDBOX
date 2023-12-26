#pragma once
#include "SoftBodyParticle.h"

namespace IHCEngine::Physics
{
	class SoftBodyParticle;

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

		Spring(const SpringType& springType, SoftBodyParticle* particle1, SoftBodyParticle* particle2, float stiffness, float damping);

		SpringType& GetSpringType() { return springType; }

		void ApplyForce(float deltaTime);

		void ApplyStretchConstraint();

	private:

		SpringType springType = SpringType::DEFAULT;
		SoftBodyParticle* p1 = nullptr;
		SoftBodyParticle* p2 = nullptr;

		float baseLength;
		float stiffness; // k
		float damping;   // d
	};

}
