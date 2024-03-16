#pragma once
#include "../../../../Engine/src/Core/Scene/Components/ImguiContextComponent.h"

namespace IHCEngine::Component
{
    class ImguiContext_GrassScene : public ImguiContextComponent
    {
    public:
        void UpdateContext() override;

    private:

        // Wind
        float windSpeed { 0.0f };
        glm::vec4 windDirection { glm::vec4(1.0)};
        float windStrength{ 0.0f };

        // Swaying
        float swayStrength{ 0.0f };
        float swayFrequency{ 0.0f };

        // Bezier curve for blade
        bool enableGlobalTilt{ false };
        float globalTilt{ 0.0f };
        bool enableControlPt{ false };
        float bend{ 0.0f };
        glm::vec4 controlPtA { glm::vec4(0.0f, 0.53f, 0.0f, 0.0f)};
        glm::vec4 controlPtB { glm::vec4(0.0f, 0.53f, 0.0f, 0.0f)};

        // Blade Properties
        bool overrideRotation{ false };
        glm::vec4 rotation { glm::vec4(1.0, 1.0, 1.0, 0.0)};
        float amountOfRotation{ 0.0f };
    };
}

