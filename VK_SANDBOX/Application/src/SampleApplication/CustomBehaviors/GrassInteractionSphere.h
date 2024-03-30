#pragma once
#include <GLFW/glfw3.h>
#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"

namespace IHCEngine::Component
{
	class ComputeGrassComponent;
}

namespace IHCEngine::Graphics
{
	class Camera;
}

namespace SampleApplication
{
    class GrassInteractionSphere : public IHCEngine::Component::CustomBehavior
    {
    public:
        void Awake() override;
        void Update() override;
        void FixedUpdate() override;

        void AddComputeGrassComponent(IHCEngine::Component::ComputeGrassComponent* c);

    private:
        IHCEngine::Graphics::Camera* camera = nullptr;

        float movementScale = 250.0f;
        glm::vec3 force = glm::vec3(0.0f);
        glm::vec3 velocity = glm::vec3(0.0f);
        float mass = 1.0f;

        std::vector<IHCEngine::Component::ComputeGrassComponent*> computeGrassComponents;
    };
}
