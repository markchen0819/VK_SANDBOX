#include "../pch.h"
#include "Particle.h"

#include <random>
namespace IHCEngine::Physics
{
    Particle::Particle(int id, const glm::vec3& position, float mass)
	: id(id), position(position), mass(mass),
	  velocity(0.0f), acceleration(0.0f), force(0.0f)
	{

	}

    void Particle::ApplyForce(const glm::vec3& f)
	{
		force += f;
	}

    void Particle::ApplyAcceleration(const glm::vec3& acceleration)
    {
        force += mass * acceleration;
    }

    glm::vec3 Particle::computeVelocity(float deltaTime)
    {
        return velocity + acceleration * deltaTime;
    }

    glm::vec3 Particle::computeAcceleration(float deltaTime, glm::vec3 const& velocity)
    {
        return force / mass;
    }

    void Particle::EulerIntegrate(float deltaTime)
    {
        if (isPinned) return;

        glm::vec3 newAcceleration = computeAcceleration(0, velocity);
        velocity += newAcceleration * deltaTime; 
        position += velocity * deltaTime; 

        force = glm::vec3(0.0f);
    }

    void Particle::VerletIntegrate(float deltaTime)
    {
        if (isPinned) return;

        glm::vec3 newAcceleration = computeAcceleration(0, velocity);

        // f (x + deltaX) = f(x) + (f(x) - f(x - deltaX))  +  f''(x) deltaX^2
        glm::vec3 newPosition = position + velocity * deltaTime + 0.5f * newAcceleration * deltaTime * deltaTime; 
        glm::vec3 newAccelerationAtNewPosition = computeAcceleration(deltaTime, newPosition);
        glm::vec3 newVelocity = velocity + 0.5f * (newAcceleration + newAccelerationAtNewPosition) * deltaTime;

        position = newPosition;
        velocity = newVelocity;

        force = glm::vec3(0.0f);
    }

    void Particle::RungeKutta2Integrate(float deltaTime)
	{
        if (isPinned) return;

        glm::vec3 newPosition;
        glm::vec3 newVelocity;
        glm::vec3 k1dv = computeAcceleration(0, velocity) * deltaTime;
        glm::vec3 midpointVelocity = velocity + 0.5f * k1dv;
        glm::vec3 k2dp = computeVelocity(0.5f * deltaTime) * deltaTime;
        glm::vec3 k2dv = computeAcceleration(0.5f * deltaTime, midpointVelocity) * deltaTime;
        // Use the midpoint values to calculate the new position and velocity
        newPosition = position + k2dp; 
        newVelocity = velocity + k2dv; 

        position = newPosition;
        velocity = newVelocity;

        force = glm::vec3(0.0f);
    }

    void Particle::RungeKutta4Integrate(float deltaTime)
	{
        if (isPinned) return;

        // Reference
        // https://www.youtube.com/watch?v=1YZnic1Ug9g
    	// https://www.intmath.com/differential-equations/12-runge-kutta-rk4-des.php

        glm::vec3 newPosition;
        glm::vec3 newVelocity;

        glm::vec3 k1dp = computeVelocity(0) * deltaTime;
        glm::vec3 k1dv = computeAcceleration(0, velocity) * deltaTime;
        glm::vec3 k2dp = computeVelocity(0.5f * deltaTime) * deltaTime;
        glm::vec3 k2dv = computeAcceleration(0.5f * deltaTime, velocity + 0.5f * k1dv) * deltaTime;
        glm::vec3 k3dp = computeVelocity(0.5f * deltaTime) * deltaTime;
        glm::vec3 k3dv = computeAcceleration(0.5f * deltaTime, velocity + 0.5f * k2dv) * deltaTime;
        glm::vec3 k4dp = computeVelocity(deltaTime) * deltaTime;
        glm::vec3 k4dv = computeAcceleration(deltaTime, this->velocity + k3dv) * deltaTime;

        newPosition = this->position + (1.0f / 6.0f) * (k1dp + 2.0f * k2dp + 2.0f * k3dp + k4dp);
        newVelocity = this->velocity + (1.0f / 6.0f) * (k1dv + 2.0f * k2dv + 2.0f * k3dv + k4dv);

        position = newPosition;
        velocity = newVelocity;

        force = glm::vec3(0.0f);
    }

}
