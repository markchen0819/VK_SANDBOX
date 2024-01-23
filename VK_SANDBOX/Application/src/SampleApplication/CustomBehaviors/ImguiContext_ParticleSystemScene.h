#pragma once
#include "../../../../Engine/src/Core/Scene/Components/ImguiContextComponent.h"

namespace IHCEngine::Component
{
    class ImguiContext_ParticleSystemScene : public ImguiContextComponent
    {
    public:
        void UpdateContext() override;

    private:
        bool enableAdvection;
        bool enableVortex;

    };
}

