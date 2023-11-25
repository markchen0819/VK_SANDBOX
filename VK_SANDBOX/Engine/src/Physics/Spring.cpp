#include "../pch.h"
#include "Spring.h"

namespace IHCEngine::Physics
{
	Spring::Spring(const std::string& springType, Particle* particle1, Particle* particle2, float k, float d)
		: springType(springType), p1(particle1), p2(particle2), stiffness(k), damping(d)
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
		glm::vec3 totalForce = calculateForce();

		// Apply forces to connected particles
		p1->ApplyForce(totalForce);
		p2->ApplyForce(-totalForce);
	}

	glm::vec3 Spring::calculateForce()
	{
		glm::vec3 deltaPosition = p2->GetPosition() - p1->GetPosition();
		glm::vec3 deltaVelocity = (p2->GetVelocity() - p1->GetVelocity());// / deltaTime;
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
