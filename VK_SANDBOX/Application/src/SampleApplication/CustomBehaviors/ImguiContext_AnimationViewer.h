#pragma once
#include "../../../../Engine/src/Core/Scene/Components/ImguiContextComponent.h"

namespace IHCEngine::Component
{
    class ImguiContext_AnimationViewer : public ImguiContextComponent
    {
    public:
        void UpdateContext() override;
    };
}
