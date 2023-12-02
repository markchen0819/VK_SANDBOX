#include "../pch.h"
#include "Particle.h"
#include "Spring.h"
namespace IHCEngine::Physics
{
    Particle::Particle(int id, const glm::vec3& position, float mass, const glm::vec3& gravity)
	: id(id), position(position), previousPosition(position),
	  velocity(0.0f), acceleration(0.0f), force(0.0f), mass(mass), gravity(gravity)
	{

	}

    void Particle::ApplyForce(const glm::vec3& f)
	{
		force += f;
	}


    void Particle::SetPinned(bool pinned)
    {
        isPinned = pinned;
    }

    void Particle::SetPosition(glm::vec3 pos)
    {
        position = pos;
    }

    void Particle::SetVelocity(glm::vec3 velocity)
    {
        this->velocity = velocity;
    }

    void Particle::AddConnectedSpring(Spring* spring)
    {
        connectedSprings.emplace_back(spring);
    }


    void Particle::EulerIntegrate(float deltaTime)
    {
        glm::vec3 newPosition;
        glm::vec3 newVelocity;

        glm::vec3 dp = computeVelocity(deltaTime) * deltaTime;
        glm::vec3 dv = computeAcceleration(deltaTime, velocity);
        newPosition = this->position + dp;
        newVelocity = this->velocity + dv;

        acceleration = (newVelocity - velocity) / deltaTime;
        position = newPosition;
        velocity = newVelocity;

        //velocity += acceleration * deltaTime;
        // += velocity * deltaTime;
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
        glm::vec3 k1v = force / mass * deltaTime;
        glm::vec3 k1x = velocity * deltaTime;

        // Estimate midpoint
        glm::vec3 midpointVelocity = velocity + k1v * 0.5f;
        glm::vec3 midpointPosition = position + k1x * 0.5f;
        glm::vec3 midpointForces = calculateForces(midpointPosition, midpointVelocity);
        glm::vec3 midpointAcceleration = midpointForces / mass;

        // Update velocity and position using midpoint acceleration
        velocity += midpointAcceleration * deltaTime;
        position += midpointVelocity * deltaTime;

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
        glm::vec3 k2dp = computeVelocity(0.5 * deltaTime) * deltaTime;
        glm::vec3 k3dp = computeVelocity(0.5 * deltaTime) * deltaTime;
        glm::vec3 k4dp = computeVelocity(deltaTime) * deltaTime;

        glm::vec3 k1dv = computeAcceleration(0, velocity) * deltaTime;
        glm::vec3 k2dv= computeAcceleration(0.5 * deltaTime, velocity + 0.5f * k1dv) * deltaTime;
        glm::vec3 k3dv = computeAcceleration(0.5 * deltaTime, velocity + 0.5f * k2dv) * deltaTime;
        glm::vec3 k4dv = computeAcceleration(deltaTime, this->velocity + k3dv) * deltaTime;

        newPosition = this->position + (1.0f / 6.0f) * (k1dp + 2.0f * k2dp + 2.0f * k3dp + k4dp);
        newVelocity = this->velocity + (1.0f / 6.0f) * (k1dv + 2.0f * k2dv + 2.0f * k3dv + k4dv);

        acceleration = (newVelocity - velocity) / deltaTime;

        position = newPosition;
        velocity = newVelocity;

        force = glm::vec3(0.0f, 0.0f, 0.0f);

  //      glm::vec3 accleration = force / mass;

  //      glm::vec3 k1dp = velocity;
  //      glm::vec3 k1dv = accleration;
  //      glm::vec3 k2dp = velocity + k1dv * deltaTime * 0.5f;
  //      glm::vec3 k2dv = accleration;
  //      glm::vec3 k3dp = velocity + k2dv * deltaTime * 0.5f;
  //      glm::vec3 k3dv = accleration;
  //      glm::vec3 k4dp = velocity + k3dv * deltaTime;
  //      glm::vec3 k4dv = accleration;

  //      velocity += (k1dv + 2.0f * k2dv + 2.0f * k3dv + k4dv) / 6.0f * deltaTime;
		//position += (k1dp + 2.0f * k2dp + 2.0f * k3dp + k4dp) / 6.0f * deltaTime;
  //      force = glm::vec3(0.0f);

        //// //k_NUM_v is delta_velocity
        //// //           = acceleration * delta_time
        //// //           = estimate changes in velocity over the timestep

        //// k1 is based on the initial state
        //glm::vec3 k1v = force / mass * deltaTime;
        //glm::vec3 k1x = velocity * deltaTime;

        //// Calculate midpoint for k2
        //glm::vec3 midpointVelocityForK2 = velocity + k1v * 0.5f;
        //glm::vec3 midpointForcesForK2 = force * 0.5f;//calculateForces(position + k1x * 0.5f, midpointVelocityForK2);
        //glm::vec3 k2v = midpointForcesForK2 / mass * deltaTime;
        //glm::vec3 k2x = midpointVelocityForK2 * deltaTime;

        //// Calculate midpoint for k3
        //glm::vec3 midpointVelocityForK3 = velocity + k2v * 0.5f;
        //glm::vec3 midpointForcesForK3 = force * 0.5f;// calculateForces(position + k2x * 0.5f, midpointVelocityForK3);
        //glm::vec3 k3v = midpointForcesForK3 / mass * deltaTime;
        //glm::vec3 k3x = midpointVelocityForK3 * deltaTime;

        //// Calculate end point for k4
        //glm::vec3 endPointVelocityForK4 = velocity + k3v;
        //glm::vec3 endPointForcesForK4 = force;// calculateForces(position + k3x, endPointVelocityForK4);
        //glm::vec3 k4v = endPointForcesForK4 / mass * deltaTime;
        //glm::vec3 k4x = endPointVelocityForK4 * deltaTime;

        //// Weighted average, combine k values to update the velocity and position
    
        //velocity += (k1v + 2.0f * k2v + 2.0f * k3v + k4v) / 6.0f;
        //position += (k1x + 2.0f * k2x + 2.0f * k3x + k4x) / 6.0f;

        //force = glm::vec3(0.0f);
    }


    glm::vec3 Particle::computeVelocity(float deltaTime)
    {
        return velocity + acceleration * deltaTime;
    }

    glm::vec3 Particle::computeAcceleration(float deltaTime, glm::vec3 const& velocity)
    {
        glm::vec3 gravityForce(0.0f, -9.81 * mass, 0.0f);
        force += gravityForce;
        return force / mass;
    }

    glm::vec3 Particle::calculateForces(const glm::vec3& pos, const glm::vec3& vel)
    {
        glm::vec3 totalForces = glm::vec3(0.0f);

        if(isPinned)
        {
            return totalForces;
        }
        // Add gravity
        totalForces += gravity * mass;

        // Add internal forces from springs
        for (const auto& spring : connectedSprings) 
        {
            glm::vec3 springForce = spring->GetForceAppliedToParticle(this, pos, vel);
            totalForces += springForce;
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
