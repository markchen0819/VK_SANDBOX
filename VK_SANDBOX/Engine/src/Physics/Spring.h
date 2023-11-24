#pragma once
#include "Particle.h"

namespace IHCEngine::Physics
{
	class Particle;

	class Spring
	{
	public:

		Spring(Particle* particle1, Particle* particle2, float k, float d);

		void ApplyInternalForce(float deltaTime);

	private:

		Particle* p1;
		Particle* p2;
		float baseLength;
		float stiffness;
		float damping;

	};

}
