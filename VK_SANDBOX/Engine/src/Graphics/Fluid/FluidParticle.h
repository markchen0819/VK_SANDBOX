#pragma once
namespace IHCEngine::Graphics
{
    struct FluidParticle
    {
        glm::vec4 position;
        glm::vec4 predictPosition; // important to make system stable
        glm::vec4 velocity;
        glm::vec4 color;
        glm::vec4 force;
        float pressure = 0.0f;
        float density = 0.0f;
    };
}