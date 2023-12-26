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
			particles.push_back(SoftBodyParticle(id, v.position, 0.5));
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
				}
				if (y < height - 1) // Below
				{
					auto spring = std::make_unique<Spring>(SpringType::ELASTIC, &particles[current], &particles[current + width], kElastic, damping);
					springs.emplace_back(std::move(spring));
				}
				// Create springs for bending connections (Bend)
				if (x < width - 2) // Two to the right
				{
					auto spring = std::make_unique<Spring>(SpringType::BEND, &particles[current], &particles[current + 2], kBend, damping);
					springs.emplace_back(std::move(spring));
				}
				if (y < height - 2) // Two below
				{
					auto spring = std::make_unique<Spring>(SpringType::BEND, &particles[current], &particles[current + 2 * width], kBend, damping);
					springs.emplace_back(std::move(spring));
				}
				// Create springs for diagonal connections (Shear)
				if (x < width - 1 && y < height - 1) // Diagonal right and below
				{ 
					auto spring = std::make_unique<Spring>(SpringType::SHEAR, &particles[current], &particles[current + width + 1], kShear, damping);
					springs.emplace_back(std::move(spring));
				}
				if (x > 0 && y < height - 1)  // Diagonal left and below
				{ 
					auto spring = std::make_unique<Spring>(SpringType::SHEAR, &particles[current], &particles[current + width - 1], kShear, damping);
					springs.emplace_back(std::move(spring));
				}
			}
		}
	}

	void SoftBody::SetPinnedParticles(int id, bool isPinned)
	{
		particles[id].SetPinned(isPinned);
	}

	glm::vec3 SoftBody::GetCenter()
	{
		size_t middleIndex = particles.size() / 2;
		return particles[middleIndex].GetPosition();
	}

	void SoftBody::ApplyForce(const glm::vec3& force)
	{
		for(auto& p : particles)
		{
			p.ApplyForce(force);
		}
	}

	void SoftBody::ApplyAcceleration(const glm::vec3& acc)
	{
		for (auto& p : particles)
		{
			p.ApplyAcceleration(acc);
		}
	}

	void SoftBody::ApplyForceOnFace(const glm::vec3& force)
	{
		for (int y = 0; y < height-1; ++y)
		{
			for (int x = 0; x < width-1; ++x)
			{
				auto& p1 = particles[y * width + x];
				auto& p2 = particles[(y + 1) * width + x];
				auto& p3 = particles[y * width + (x + 1)];
				auto& p4 = particles[(y + 1) * width + (x + 1)];

				// First Triangle of Square
				glm::vec3 v1 = p2.GetPosition() - p1.GetPosition();
				glm::vec3 v2 = p3.GetPosition() - p1.GetPosition();
				glm::vec3 normal = glm::normalize(glm::cross(v1,v2));
				glm::vec3 f = normal * glm::dot(normal, force);
				p1.ApplyForce(f);
				p2.ApplyForce(f);
				p3.ApplyForce(f);

				// Second Triangle of Square
				v1 = p4.GetPosition() - p3.GetPosition();
				v2 = p4.GetPosition() - p2.GetPosition();
				normal = glm::normalize(glm::cross(v1, v2));
				f = normal * glm::dot(normal, force);
				p4.ApplyForce(f);
				p3.ApplyForce(f);
				p2.ApplyForce(f);
			}
		}
	}

	void SoftBody::CheckCollisionFromSphere(glm::vec3 sphereCenter, float radius)
	{
		for (auto& p : particles)
		{
			if(p.IsPinned())
			{
				continue;
			}
			auto vec = p.GetPosition() - sphereCenter;
			float distanceToSphereCenter = glm::length(vec);

			if (distanceToSphereCenter < radius)
			{
				glm::vec3 correctionVector = (radius - distanceToSphereCenter) * glm::normalize(vec);
				p.SetPosition(p.GetPosition() + correctionVector);
			}
		}
	}

	void SoftBody::SetSphere(glm::vec3 pos, float radius)
	{
		sphereCenterPosition = pos;
		sphereRadius = radius;
	}

	void SoftBody::Update()
	{
		float dt = IHCEngine::Core::Time::GetDeltaTime();

		// Internal Forces
		for (auto& spring : springs) 
		{
			spring->ApplyForce(dt);
		}

		// Integrate to get new positions and velocities
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

		// Handle constraints and collisions
		int iterations = 3;
		// more iterations for more correct constraint
		// but performance heavy and potential fps degrade
		for(int i=0; i< iterations; ++i)
		{
			for (auto& spring : springs)
			{
				spring->ApplyStretchConstraint();
			}
		}

		CheckCollisionFromSphere(sphereCenterPosition, sphereRadius);

		// Render
		auto& vertices = mesh->GetVertices();
		for(int i=0; i < vertices.size(); ++i)
		{
			vertices[i].position = particles[i].GetPosition();
		}
		mesh->UpdateVertices();
	}

	void SoftBody::SetIntegrationMethod(IntegrationMethod method)
	{
		integrationMethod = method;
	}
}
