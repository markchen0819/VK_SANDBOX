#include "../pch.h"
#include "PhysicsSolver.h"

#include "../Core/Scene/Components/MeshComponent.h"
#include "../Graphics/VKWraps/IHCMesh.h"
#include "../Graphics/VKWraps/VKHelpers.h"
#include "../Core/Time/Time.h"

namespace IHCEngine::Physics
{
	PhysicsSolver::PhysicsSolver()
		: gravity(glm::vec3(0.0f, -0.981f, 0.0f))
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

		for (int y = 0; y < gridHeight; ++y)
		{
			for (int x = 0; x < gridWidth; ++x)
			{
				int current = y * dimension + x;

				// Create springs for direct connections (Elastic)
				if (x < dimension - 1)  // To the right
				{
					auto spring = std::make_unique<Spring>("Elastic", &particles[current], &particles[current + 1], kElastic, damping);
					springs.emplace_back(std::move(spring));
					particles[current].AddConnectedSpring(springs.back().get());
					particles[current + 1].AddConnectedSpring(springs.back().get());
				}
				if (y < dimension - 1) // Below
				{
					auto spring = std::make_unique<Spring>("Elastic", &particles[current], &particles[current + gridWidth], kElastic, damping);
					springs.emplace_back(std::move(spring));
					particles[current].AddConnectedSpring(springs.back().get());
					particles[current + gridWidth].AddConnectedSpring(springs.back().get());
				}

				// Create springs for bending connections (Bend)
				if (x < gridWidth - 2) // Two to the right
				{
					auto spring = std::make_unique<Spring>("Bend", &particles[current], &particles[current + 2], kBend, damping);
					springs.emplace_back(std::move(spring));
					particles[current].AddConnectedSpring(springs.back().get());
					particles[current + 2].AddConnectedSpring(springs.back().get());
				}
				if (y < gridHeight - 2) // Two below
				{
					auto spring = std::make_unique<Spring>("Bend", &particles[current], &particles[current + 2 * gridWidth], kBend, damping);
					springs.emplace_back(std::move(spring));
					particles[current].AddConnectedSpring(springs.back().get());
					particles[current + 2 * gridWidth].AddConnectedSpring(springs.back().get());
				}

				// Create springs for diagonal connections (Shear)
				if (x < gridWidth - 1 && y < gridHeight - 1) // Diagonal right and below
				{ 
					auto spring = std::make_unique<Spring>("Shear", &particles[current], &particles[current + gridWidth + 1], kShear, damping);
					springs.emplace_back(std::move(spring));
					particles[current].AddConnectedSpring(springs.back().get());
					particles[current + gridWidth + 1].AddConnectedSpring(springs.back().get());
				}
				if (x > 0 && y < gridHeight - 1)  // Diagonal left and below
				{ 
					auto spring = std::make_unique<Spring>("Shear", &particles[current], &particles[current + gridWidth - 1], kShear, damping);
					springs.emplace_back(std::move(spring));
					particles[current].AddConnectedSpring(springs.back().get());
					particles[current + gridWidth - 1].AddConnectedSpring(springs.back().get());
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

		float dt = IHCEngine::Core::Time::GetDeltaTime();

		// External Forces
		for (auto& p : particles) 
		{
			glm::vec3 totalExternalForces = gravity * p.GetMass(); // Gravity
			//totalExternalForces += windForce;
			p.ApplyForce(gravity * p.GetMass());
		}

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
