#include "../pch.h"
#include "Particle.h"
#include "Spring.h"
namespace IHCEngine::Physics
{
    Particle::Particle(int id, const glm::vec3& position, float mass, const glm::vec3& gravity)
	: id(id), position(position), previousPosition(position),
	  velocity(0.0f), force(0.0f), mass(mass), gravity(gravity)
	{

	}

    void Particle::ApplyForce(const glm::vec3& f)
	{
        if (isPinned)
        {
            return;
        }
		force += f;
	}


    void Particle::SetPinned(bool pinned)
    {
        isPinned = pinned;
    }

    void Particle::EulerIntegrate(float deltaTime)
    {
        acceleration = force / mass;
        velocity += acceleration * deltaTime;
        position += velocity * deltaTime;

        force = glm::vec3(0.0f);
    }

    void Particle::VerletIntegrate(float deltaTime)
    {
        // f (x + deltaX) = f(x) + (f(x) - f(x - deltaX))  +  f''(x) deltaX^2
        acceleration = force / mass;
    	const glm::vec3 currentPosition = position;
        velocity = (currentPosition - previousPosition) / deltaTime;
        position = currentPosition + velocity * deltaTime + acceleration * deltaTime * deltaTime;
        previousPosition = currentPosition;

        force = glm::vec3(0.0f);
    }

    void Particle::RungeKutta2Integrate(float deltaTime)
	{
        // Initial state
        glm::vec3 initialAcceleration = force / mass;
        glm::vec3 initialVelocity = velocity;

        // Estimate midpoint
        glm::vec3 midpointVelocity = velocity + 0.5f * deltaTime * initialAcceleration;
        glm::vec3 midpointPosition = position + 0.5f * deltaTime * initialVelocity;
        glm::vec3 midpointForces = calculateForces(midpointPosition, midpointVelocity);
        glm::vec3 midpointAcceleration = midpointForces / mass;

        // Update velocity and position using midpoint acceleration
        velocity += deltaTime * midpointAcceleration;
        position += deltaTime * midpointVelocity;

        force = glm::vec3(0.0f);
    }

    void Particle::RungeKutta4Integrate(float deltaTime)
	{
        // k1 is based on the initial state
        glm::vec3 k1v = force / mass * deltaTime;
        glm::vec3 k1x = velocity * deltaTime;

        // Calculate midpoint for k2
        glm::vec3 midpointVelocityForK2 = velocity + k1v * 0.5f;
        glm::vec3 midpointForcesForK2 = calculateForces(position + k1x * 0.5f, midpointVelocityForK2);
        glm::vec3 k2v = midpointForcesForK2 / mass * deltaTime;
        glm::vec3 k2x = midpointVelocityForK2 * deltaTime;

        // Calculate midpoint for k3
        glm::vec3 midpointVelocityForK3 = velocity + k2v * 0.5f;
        glm::vec3 midpointForcesForK3 = calculateForces(position + k2x * 0.5f, midpointVelocityForK3);
        glm::vec3 k3v = midpointForcesForK3 / mass * deltaTime;
        glm::vec3 k3x = midpointVelocityForK3 * deltaTime;

        // Calculate end point for k4
        glm::vec3 endPointVelocityForK4 = velocity + k3v;
        glm::vec3 endPointForcesForK4 = calculateForces(position + k3x, endPointVelocityForK4);
        glm::vec3 k4v = endPointForcesForK4 / mass * deltaTime;
        glm::vec3 k4x = endPointVelocityForK4 * deltaTime;

        // Combine k values to update the velocity and position
        velocity += (k1v + 2.0f * k2v + 2.0f * k3v + k4v) / 6.0f;
        position += (k1x + 2.0f * k2x + 2.0f * k3x + k4x) / 6.0f;

        force = glm::vec3(0.0f);
    }

    void Particle::AddConnectedSpring(Spring* spring)
    {
        connectedSprings.emplace_back(spring);
    }

    glm::vec3 Particle::calculateForces(const glm::vec3& pos, const glm::vec3& vel)
    {
        glm::vec3 totalForces = glm::vec3(0.0f);

        // Add gravity force
        totalForces += gravity * mass;

        // Add forces from springs
        for (const auto& spring : connectedSprings) 
        {
            //Particle* otherParticle = spring.GetOtherParticle(this);
            //glm::vec3 otherPos = otherParticle->GetPosition();
            //glm::vec3 springForce = /* Calculate spring force based on pos and otherPos */;
            //totalForces += springForce;
        }

        //// Check for and respond to collisions
        //if (/* Check for collision with other objects */) {
        //    glm::vec3 collisionResponseForce = /* Calculate collision response force */;
        //    totalForces += collisionResponseForce;
        //}

        // Add any other forces relevant to your simulation
        // ...

        return totalForces;
    }

}
