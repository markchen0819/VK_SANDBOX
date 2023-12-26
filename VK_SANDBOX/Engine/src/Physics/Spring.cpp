#include "../pch.h"
#include "Spring.h"

namespace IHCEngine::Physics
{
	Spring::Spring(const SpringType& springType, SoftBodyParticle* particle1, SoftBodyParticle* particle2, float stiffness, float damping)
		: springType(springType), p1(particle1), p2(particle2), stiffness(stiffness), damping(damping)
	{
		baseLength = glm::length(p2->GetPosition() - p1->GetPosition());
	}

	void Spring::ApplyForce(float deltaTime)
	{
		glm::vec3 deltaPosition = p1->GetPosition() - p2->GetPosition();
		float stretchedLength = glm::length(deltaPosition);
		glm::vec3 forceDirection = glm::normalize(deltaPosition);

		glm::vec3 springForce; // Hooke's Law
		if (stretchedLength == 0) 
		{
			springForce = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		else 
		{
			float deltaLength;
			if (stretchedLength > baseLength)
			{
				// limits the maximum stretch to 10% more than the baseLength
				deltaLength = std::min<float>(stretchedLength - baseLength, (1.1f * baseLength) - baseLength);
			}
			else 
			{
				// limits the maximum compression to 10 % less than the restLength
				deltaLength = std::max<float>(stretchedLength - baseLength, (0.9f * baseLength) - baseLength);
			}

			springForce = stiffness * forceDirection * (deltaLength);
		}

		glm::vec3 dampForce1 = -damping * p1->GetVelocity();
		glm::vec3 dampForce2 = -damping * p2->GetVelocity();

		p1->ApplyForce(-springForce + dampForce1);
		p2->ApplyForce(springForce + dampForce2);
	}

	void Spring::ApplyStretchConstraint()
	{
		if (p1->IsPinned() && p2->IsPinned())
		{
			return;
		}

		glm::vec3 deltaPosition = p2->GetPosition() - p1->GetPosition();
		float stretchedLength = glm::length(deltaPosition);

		// Calculate the correction required to satisfy the constraint
		glm::vec3 correctionVector = deltaPosition / stretchedLength * (stretchedLength - baseLength);

		// Apply half of the correction to each particle, unless one is pinned
		glm::vec3 halfCorrection = correctionVector * 0.5f;
		if (!p1->IsPinned() && !p2->IsPinned())
		{
			p1->SetPosition(p1->GetPosition() + halfCorrection);
			p2->SetPosition(p2->GetPosition() - halfCorrection);
		}
		else if (!p1->IsPinned())
		{
			p1->SetPosition(p1->GetPosition() + correctionVector);
		}
		else if (!p2->IsPinned())
		{
			p2->SetPosition(p2->GetPosition() - correctionVector);
		}
	}
}
