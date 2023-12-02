#include "../pch.h"
#include "Spring.h"

namespace IHCEngine::Physics
{
	Spring::Spring(const SpringType& springType, Particle* particle1, Particle* particle2, float stiffness, float damping)
		: springType(springType), p1(particle1), p2(particle2), stiffness(stiffness), damping(damping)
	{
		baseLength = glm::length(p2->GetPosition() - p1->GetPosition());
	}

	glm::vec3 Spring::GetForceAppliedToParticle(Particle* targetParticle, const glm::vec3& pos, const glm::vec3& vel)
	{
		if(targetParticle->GetID() == p1->GetID())
		{
			glm::vec3 deltaPosition = p2->GetPosition() - pos;
			glm::vec3 deltaVelocity = (p2->GetVelocity() - vel);
			float stretchedLength = glm::length(deltaPosition);
			glm::vec3 forceDirection = glm::normalize(deltaPosition);
			float stiffTerm = stiffness * (stretchedLength - baseLength);
			float dampTerm = damping * glm::dot(deltaVelocity, forceDirection);
			glm::vec3 totalForce = (stiffTerm + dampTerm) * forceDirection;
			return totalForce;
		}
		else if(targetParticle->GetID() == p2->GetID())
		{
			glm::vec3 deltaPosition = pos - p1->GetPosition();
			glm::vec3 deltaVelocity = vel - p1->GetVelocity();
			float stretchedLength = glm::length(deltaPosition);
			glm::vec3 forceDirection = glm::normalize(deltaPosition);
			float stiffTerm = stiffness * (stretchedLength - baseLength);
			float dampTerm = damping * glm::dot(deltaVelocity, forceDirection);
			glm::vec3 totalForce = (stiffTerm + dampTerm) * forceDirection;
			return -totalForce;
		}
		else
		{
			std::cerr << "Target particle is not assigned to this spring" << std::endl;
			assert(false);
		}
		return glm::vec3(0,0,0);
	}

	void Spring::ApplyForce(float deltaTime)
	{
		//glm::vec3 totalForce = calculateForce();

		//// Apply forces to connected particles
		//p1->ApplyForce(totalForce);


		//p2->ApplyForce(-totalForce);

		glm::vec3 deltaPosition = p1->GetPosition() - p2->GetPosition();
		float stretchedLength = glm::length(deltaPosition);
		glm::vec3 forceDirection = glm::normalize(deltaPosition);


		glm::vec3 springForce;
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
				deltaLength = std::max<float>(stretchedLength - baseLength, (0.9 * baseLength) - baseLength);
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
		glm::vec3 forceDirection = glm::normalize(deltaPosition);

		float stretchFactor = 1.0001f;
		float compressFactor = 0.9999f;
		if (stretchFactor * baseLength < stretchedLength)
		{
			float offset = stretchedLength - stretchFactor * baseLength;

			if (!p1->IsPinned() && !p2->IsPinned()) // both not pinned, share offset
			{
				offset *= 0.5f;
				p1->SetPosition(p1->GetPosition() + offset * forceDirection);
				p2->SetPosition(p2->GetPosition() - offset * forceDirection);
				return;
			}
			// only apply on non-pinned one
			if (!p1->IsPinned())
			{
				p1->SetPosition(p1->GetPosition() + offset * forceDirection);
				return;
			}
			if (!p2->IsPinned())
			{
				p2->SetPosition(p2->GetPosition() + offset * forceDirection);
				return;
			}
		}
		else if (compressFactor * baseLength > stretchedLength)
		{
			float offset = 0.9999f * baseLength - stretchedLength;

			if (!p1->IsPinned() && !p2->IsPinned()) // both not pinned, share offset
			{
				offset *= 0.5f;
				p1->SetPosition(p1->GetPosition() - offset * forceDirection);
				p2->SetPosition(p2->GetPosition() + offset * forceDirection);
				return;
			}
			// only apply on non-pinned one
			if (!p1->IsPinned())
			{
				p1->SetPosition(p1->GetPosition() - offset * forceDirection);
				return;
			}
			if (!p2->IsPinned())
			{
				p2->SetPosition(p2->GetPosition() + offset * forceDirection);
				return;
			}


		}

	}

	glm::vec3 Spring::calculateForce()
	{
		glm::vec3 deltaPosition = p2->GetPosition() - p1->GetPosition();
		glm::vec3 deltaVelocity = (p2->GetVelocity() - p1->GetVelocity());
		float stretchedLength = glm::length(deltaPosition);
		glm::vec3 forceDirection = glm::normalize(deltaPosition);


		//Hooke's Law for stiffness: F = -kx
		float stiffTerm = stiffness * (stretchedLength - baseLength); // Stiffness term (F = -kx)
		//Dot product of relative velocity and displacement divided by distance for damping
		float dampTerm = damping * glm::dot(deltaVelocity, forceDirection);

		// Calculate the total force by adding stiffness and damping components
		glm::vec3 totalForce = (stiffTerm + dampTerm) * forceDirection;

		return totalForce;

	}
}
