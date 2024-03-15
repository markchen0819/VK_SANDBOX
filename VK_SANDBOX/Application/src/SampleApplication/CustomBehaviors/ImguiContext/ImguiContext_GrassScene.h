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
        float windSpeed { 0.1f };
        glm::vec4 windDirection { glm::vec4(1.0)};
        float windStrength{ 0.0f };

        // Bezier curve for blade
        float bend{ 0.0f };
        glm::vec4 controlPtA { glm::vec4(1.0)};
        glm::vec4 controlPtB { glm::vec4(1.0)};

        // Swaying
        float swayStrength{ 0.0f };
        float swayFrequency{ 0.0f };

        // Blade Properties
        bool overrideScale{ false };
        glm::vec4 scale { glm::vec4(1.0, 1.0, 1.0, 0.0)};
        bool overrideRotation{ false };
        glm::vec4 rotation { glm::vec4(1.0, 1.0, 1.0, 0.0)};

        // Show Normal
        bool showWorldNormal{ false };
    };
}

