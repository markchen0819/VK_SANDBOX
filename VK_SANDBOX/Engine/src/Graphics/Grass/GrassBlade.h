#pragma once
namespace IHCEngine::Graphics
{
    struct GrassBlade
    {
        glm::vec4 position;
        glm::vec4 rotation;
        glm::vec4 scale;
        glm::vec4 color;
        float windStrength = 0.0f;
        float tilt = 30.0f;
        unsigned int perBladeHash = 0;
    };
}