#include "../pch.h"
#include "SoftBody.h"

#include "../Core/Scene/Components/MeshComponent.h"
#include "../Graphics/VKWraps/IHCMesh.h"
#include "../Graphics/VKWraps/VKHelpers.h"
#include "../Core/Time/Time.h"

namespace IHCEngine::Physics
{
	void SoftBody::SetTargetMesh(int width, int height, Component::MeshComponent* meshComponent)
	{
		this->width = width;
		this->height = height;
		mesh = meshComponent->GetMesh();
	}

	void SoftBody::SetupParticles()
	{
		auto& vertices = mesh->GetVertices();
		int id = 0;
		for(auto& v : vertices)
		{
			particles.push_back(Particle(id, v.position, 0.05, gravity));
			++id;
		}
	}

	void SoftBody::SetupSprings(float kElastic, float kBend, float kShear, float damping)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				int current = y * height + x;

				// Create springs for direct connections (Elastic)
				if (x < width - 1)  // To the right
				{
					auto spring = std::make_unique<Spring>(SpringType::ELASTIC, &particles[current], &particles[current + 1], kElastic, damping);
					springs.emplace_back(std::move(spring));
					particles[current].AddConnectedSpring(springs.back().get());
					particles[current + 1].AddConnectedSpring(springs.back().get());
				}
				if (y < height - 1) // Below
				{
					auto spring = std::make_unique<Spring>(SpringType::ELASTIC, &particles[current], &particles[current + width], kElastic, damping);
					springs.emplace_back(std::move(spring));
					particles[current].AddConnectedSpring(springs.back().get());
					particles[current + width].AddConnectedSpring(springs.back().get());
				}

				// Create springs for bending connections (Bend)
				if (x < width - 2) // Two to the right
				{
					auto spring = std::make_unique<Spring>(SpringType::BEND, &particles[current], &particles[current + 2], kBend, damping);
					springs.emplace_back(std::move(spring));
					particles[current].AddConnectedSpring(springs.back().get());
					particles[current + 2].AddConnectedSpring(springs.back().get());
				}
				if (y < height - 2) // Two below
				{
					auto spring = std::make_unique<Spring>(SpringType::BEND, &particles[current], &particles[current + 2 * width], kBend, damping);
					springs.emplace_back(std::move(spring));
					particles[current].AddConnectedSpring(springs.back().get());
					particles[current + 2 * width].AddConnectedSpring(springs.back().get());
				}

				// Create springs for diagonal connections (Shear)
				if (x < width - 1 && y < height - 1) // Diagonal right and below
				{ 
					auto spring = std::make_unique<Spring>(SpringType::SHEAR, &particles[current], &particles[current + width + 1], kShear, damping);
					springs.emplace_back(std::move(spring));
					particles[current].AddConnectedSpring(springs.back().get());
					particles[current + width + 1].AddConnectedSpring(springs.back().get());
				}
				if (x > 0 && y < height - 1)  // Diagonal left and below
				{ 
					auto spring = std::make_unique<Spring>(SpringType::SHEAR, &particles[current], &particles[current + width - 1], kShear, damping);
					springs.emplace_back(std::move(spring));
					particles[current].AddConnectedSpring(springs.back().get());
					particles[current + width - 1].AddConnectedSpring(springs.back().get());
				}
			}
		}

	}

	void SoftBody::SetPinnedParticles(int id, bool isPinned)
	{
		particles[id].SetPinned(isPinned);
	}

	void SoftBody::SetSphere(glm::vec3 pos, float radius)
	{
		sphereCenterPosition = pos;
		sphereRadius = radius;
	}

	void SoftBody::ApplyForce(const glm::vec3& force)
	{
		for (auto& p : particles)
		{
			p.ApplyForce(force);
		}
	}

	void SoftBody::Update()
	{
		float dt = IHCEngine::Core::Time::GetDeltaTime();

		//// External Forces
		//for (auto& p : particles) 
		//{
		//	glm::vec3 totalExternalForces = gravity * p.GetMass(); // Gravity
		//	//totalExternalForces += windForce;
		//	p.ApplyForce(gravity * p.GetMass());

		//}

		// Internal Forces
		for (auto& spring : springs) 
		{
			spring->ApplyForce(dt);
		}

		// Step 3: Integrate to get new positions and velocities
	
		for (auto& p : particles)
		{
			switch (integrationMethod)
			{
			case IntegrationMethod::Euler:
				p.EulerIntegrate(dt);
				break;
			case IntegrationMethod::Verlet:
				p.VerletIntegrate(dt);
				break;
			case IntegrationMethod::RungeKutta2:
				p.RungeKutta2Integrate(dt);
				break;
			case IntegrationMethod::RungeKutta4:
				p.RungeKutta4Integrate(dt);
			}
		}

		//// Step 3: Handle collisions and constraints
		///
		for (auto& spring : springs)
		{
			if (spring->GetSpringType() != SpringType::BEND)
			{
				spring->ApplyStretchConstraint();
			}
		}
		for (auto& p : particles)
		{
			fixPositionFromCollision(p);

			//float energyThreshold = 0.007;
			//if (glm::length(p.GetVelocity()) < energyThreshold)
			//{
			//	p.SetVelocity(glm::vec3(0));
			//}
		}


		// Render
		auto& vertices = mesh->GetVertices();
		for(int i=0; i < vertices.size(); ++i)
		{
			vertices[i].position = particles[i].GetPosition();
		}
		mesh->UpdateVertices();
	}

	void SoftBody::fixPositionFromCollision(Particle& particle)
	{
		glm::vec3 vec = particle.GetPosition() - sphereCenterPosition;
		float distanceToSphereCenter = glm::length(vec);

		if(distanceToSphereCenter < sphereRadius) // Collide
		{
			// Move point out of the sphere
			glm::vec3 displacementToSurface = (sphereRadius - distanceToSphereCenter) * glm::normalize(vec);
			particle.SetPosition(particle.GetPosition() + displacementToSurface);


			// Bounce back velocity with restitution
			float cor = 0.8f; // Coefficient of restitution, adjust as needed (0 = perfect inelastic, 1 = perfect elastic)
			glm::vec3 normal = glm::normalize(vec);
			glm::vec3 velocityTowardsSphere = glm::dot(particle.GetVelocity(), normal) * normal;
			glm::vec3 reflectedVelocity = particle.GetVelocity() - (1.0f + cor) * velocityTowardsSphere;
			particle.SetVelocity(reflectedVelocity);

			//glm::vec3 velocityTowardsSphere = glm::dot(particle.GetVelocity(), vec) * vec;
			//glm::vec3 reflectedVelocity = particle.GetVelocity() - glm::vec3(1.01, 1.01, 1.01) * velocityTowardsSphere;
			//particle.SetVelocity(glm::vec3(0,0,0));
		}
	}
}
