#include "../pch.h"
#include "Spring.h"

namespace IHCEngine::Physics
{
	Spring::Spring(Particle* particle1, Particle* particle2, float k, float d)
		: p1(particle1), p2(particle2), stiffness(k), damping(d)
	{
		baseLength = glm::length(p2->GetPosition() - p1->GetPosition());
	}

	void Spring::ApplyInternalForce(float deltaTime)
	{
		//// working 1
		//glm::vec3 deltaPosition = p2->GetPosition() - p1->GetPosition();
		//glm::vec3 deltaVelocity = (p2->GetVelocity() - p1->GetVelocity());// / deltaTime;
		//float stretchedLength = glm::length(deltaPosition);
		//glm::vec3 forceDirection = glm::normalize(deltaPosition);


		////Hooke's Law for stiffness: F = -kx
		//float stiffTerm = stiffness * (stretchedLength - baseLength); // Stiffness term (F = -kx)
		////Dot product of relative velocity and displacement divided by distance for damping
		//float dampTerm = damping * glm::dot(deltaVelocity, forceDirection);

		//// Calculate the total force by adding stiffness and damping components
		//glm::vec3 totalForce = (stiffTerm + dampTerm) * forceDirection;

		//// Apply forces to connected particles
		//p1->ApplyForce(totalForce);
		//p2->ApplyForce(-totalForce);


		// working 2
		glm::vec3 force = p2->GetPosition() - p1->GetPosition();
		glm::vec3 forceDirection = glm::normalize(force);
		float stretchedLength = glm::length(force);

		//// Hooke's Law for spring force: F = -kx
		glm::vec3 springForce = stiffness * (stretchedLength - baseLength) * forceDirection;

		//// Damping based on relative velocities: Fd = -d(v2 - v1)
		glm::vec3 dampingForce = damping * (p2->GetVelocity() - p1->GetVelocity());

		//// Apply forces to connected particles
		p1->ApplyForce(springForce + dampingForce);
		p2->ApplyForce(-springForce - dampingForce);

		////constraint

		//glm::vec3 deltaPosition = p2->GetPosition() - p1->GetPosition();
		//glm::vec3 forceDirection = glm::normalize(deltaPosition);
		//float stretchedLength = glm::length(deltaPosition);


		//// Define the maximum stretch length (e.g., 120% of the base length)
		//float maxStretchLength = baseLength * 1.20f;
		//// Calculate the spring force
		//float effectiveLength = stretchedLength;
		//if (stretchedLength > maxStretchLength) 
		//{
		//	effectiveLength = maxStretchLength; // Use max stretch length for force calculation
		//}

		//// Continue with the usual spring force calculation
		//glm::vec3 springForce = stiffness * (effectiveLength - baseLength) * forceDirection;
		//glm::vec3 dampingForce = damping * (p2->GetVelocity() - p1->GetVelocity());

		//// Apply the forces
		//p1->ApplyForce(springForce + dampingForce);
		//p2->ApplyForce(-springForce - dampingForce);

	}
}
