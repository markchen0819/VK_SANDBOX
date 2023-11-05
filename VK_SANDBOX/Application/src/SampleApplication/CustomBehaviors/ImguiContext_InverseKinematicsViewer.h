#pragma once
#include "../../../../Engine/src/Core/Scene/Components/ImguiContextComponent.h"

namespace IHCEngine::Component
{
    class ImguiContext_InverseKinematicsViewer : public ImguiContextComponent
    {
    public:
        void UpdateContext() override;
    };
}

