#include "ImguiContext_PhysicallyBasedSimulationViewer.h"

#include "imgui.h"
#include "MotionAlongPathViewer.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Graphics/RenderSystems/RenderSystem.h"

namespace IHCEngine::Component
{
    void ImguiContext_PhysicallyBasedSimulationViewer::UpdateContext()
    {
        ImGui::Text("-----------------------");
        ImGui::Checkbox("wireframeEnabled", &IHCEngine::Graphics::RenderSystem::wireframeEnabled);
        ImGui::Checkbox("debugBonesEnabled", &IHCEngine::Graphics::RenderSystem::debugBonesEnabled);
        ImGui::Checkbox("animationMeshEnabled", &IHCEngine::Graphics::RenderSystem::animationMeshEnabled);
    	ImGui::Text("-----------------------");
    }
}
