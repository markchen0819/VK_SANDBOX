#include "ImguiContext_InverseKinematicsViewer.h"


#include "imgui.h"
#include "InverseKinematicsViewer.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Graphics/RenderSystems/RenderSystem.h"
#include "../../../../Engine/src/Graphics/Animation/AnimationConfig.h"

namespace IHCEngine::Component
{
    void ImguiContext_InverseKinematicsViewer::UpdateContext()
    {
        ImGui::Text("-----------------------");
    }
}
