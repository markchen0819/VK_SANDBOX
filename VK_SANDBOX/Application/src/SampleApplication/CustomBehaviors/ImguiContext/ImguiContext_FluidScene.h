#pragma once
#include "../../../../Engine/src/Core/Scene/Components/ImguiContextComponent.h"

namespace IHCEngine::Component
{
    class ImguiContext_FluidScene : public ImguiContextComponent
    {
    public:
        void UpdateContext() override;

    private:
        float gasConstant = 0.0f;
        float restDensity = 0.0f;
        float viscosity = 0.0f;
    };
}

