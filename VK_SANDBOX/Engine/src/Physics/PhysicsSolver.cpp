#include "../pch.h"
#include "PhysicsSolver.h"

#include "../Core/Scene/Components/MeshComponent.h"
#include "../Graphics/VKWraps/IHCMesh.h"
#include "../Graphics/VKWraps/VKHelpers.h"
#include "../Core/Time/Time.h"

namespace IHCEngine::Physics
{
	PhysicsSolver::PhysicsSolver()
		: gravity(glm::vec3(0.0f, -0.0981f, 0.0f))
	{

	}

	PhysicsSolver::~PhysicsSolver()
	{
	}

	void PhysicsSolver::SetTargetMesh(Component::MeshComponent* meshComponent)
	{
		mesh = meshComponent->GetMesh();
	}

	void PhysicsSolver::SetupParticles()
	{
		auto& vertices = mesh->GetVertices();

		int id = 0;
		for(auto& v : vertices)
		{
			particles.push_back(Particle(id, v.position, 1.0, gravity));
			++id;
		}
	}

	void PhysicsSolver::SetupSprings(int dimension, float kElastic, float kBend, float kShear, float damping)
	{
		float gridHeight = dimension;
		float gridWidth = dimension;
		//float damping = 15;// verlet
		//float damping = 0.9; //euler

		for (int y = 0; y < gridHeight; ++y)
		{
			for (int x = 0; x < gridWidth; ++x)
			{
				int current = y * dimension + x;

				// Create springs for direct connections (Elastic)
				if (x < dimension - 1)  // To the right
				{ 
					springs.emplace_back(Spring(& particles[current], &particles[current + 1], kElastic, damping));
				}
				if (y < dimension - 1) // Below
				{ 
					springs.emplace_back(Spring(&particles[current], &particles[current + gridWidth], kElastic, damping));
				}

				// Create springs for bending connections (Bend)
				if (x < gridWidth - 2) // Two to the right
				{
					springs.emplace_back(Spring(&particles[current], &particles[current + 2], kBend, damping));
				}
				if (y < gridHeight - 2) // Two below
				{
					springs.emplace_back(Spring(&particles[current], &particles[current + 2 * gridWidth], kBend, damping));
				}

				// Create springs for diagonal connections (Shear)
				if (x < gridWidth - 1 && y < gridHeight - 1) // Diagonal right and below
				{ 
					springs.emplace_back(Spring(&particles[current], &particles[current + gridWidth + 1], kShear, damping));
				}
				if (x > 0 && y < gridHeight - 1)  // Diagonal left and below
				{ 
					springs.emplace_back(Spring(&particles[current], &particles[current + gridWidth - 1], kShear, damping));
				}
			}
		}

	}

	void PhysicsSolver::SetPinnedParticles(int id, bool isPinned)
	{
		particles[id].SetPinned(isPinned);
	}

	void PhysicsSolver::Update()
	{

		//// Define the wind force direction and magnitude
		//glm::vec2 baseWindDirection = glm::vec2(1.0f, 0.0f); // Wind blowing along the x-axis in 2D
		//float windMagnitude = 1.0f; // Adjust the magnitude to simulate stronger or weaker wind

		//// Randomness factors for variability in strength and direction
		//float windVariabilityStrength = 1.5f;
		//float windVariabilityDirection = 100.0f; // Determines how much the wind direction can change

		//// Calculate the random wind strength
		//float windStrength = windMagnitude * (1.0f + windVariabilityStrength * ((rand() / (float)RAND_MAX) - 0.5f));

		//// Calculate the random wind direction change in radians
		//float windDirectionChangeRadians = windVariabilityDirection * ((rand() / (float)RAND_MAX) - 0.5f) * 2 * glm::pi<float>();

		//// Rotate the base wind direction by the random angle
		//glm::vec2 randomWindDirection = glm::vec2(
		//	baseWindDirection.x * cos(windDirectionChangeRadians) - baseWindDirection.y * sin(windDirectionChangeRadians),
		//	baseWindDirection.x * sin(windDirectionChangeRadians) + baseWindDirection.y * cos(windDirectionChangeRadians)
		//);

		//// Normalize the final wind direction and convert to 3D
		//glm::vec3 windDirection = glm::normalize(glm::vec3(randomWindDirection, 0.0f));

		//// Calculate the final wind force vector
		//glm::vec3 windForce = windStrength * windDirection;

		float dt = IHCEngine::Core::Time::GetDeltaTime();
		// Compute external forces 
		for (auto& p : particles) 
		{
			glm::vec3 totalExternalForces = gravity * p.GetMass(); // Gravity
			//totalExternalForces += windForce;
			p.ApplyForce(gravity * p.GetMass());
		}


		for (auto& s : springs) 
		{
			s.ApplyInternalForce(dt);
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

		// Step 3: Handle collisions and constraints



		// Render
		auto& vertices = mesh->GetVertices();
		for(int i=0; i < vertices.size(); ++i)
		{
			vertices[i].position = particles[i].GetPosition();
		}
		mesh->UpdateVertices();
	}
}
