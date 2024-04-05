#include "ImguiContext_FluidScene.h"

#include "imgui.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"

namespace IHCEngine::Component
{
    void ImguiContext_FluidScene::UpdateContext()
    {
        auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
        auto scene = sceneManager->GetActiveScene();
        auto camera = scene->GetGameObjectByName("camera");

        ImGui::Text("------------------------");
        ImGui::Text(" Interface");
        ImGui::Text(" WSAD/ Shift/ Control : Move Camera");
        ImGui::Text(" Mouse Right Click Drag: Rotate Camera");
        ImGui::Text(" Mouse Scroll: Zoom In/Out");
        ImGui::Text(" F7: Full scene Reset");
        ImGui::Text("------------------------");

    }

}
