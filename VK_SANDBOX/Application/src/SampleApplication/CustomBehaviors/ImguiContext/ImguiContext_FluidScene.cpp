#include "ImguiContext_FluidScene.h"

#include "imgui.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Core/Scene/Components/ComputeFluidComponent.h"


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

        auto gobj = scene->GetGameObjectByName("fluidGobj");
        auto computeFluid = gobj->GetComponent<ComputeFluidComponent>();

        gasConstant = computeFluid->GetGasConstant();
        restDensity = computeFluid->GetRestDensity();
        viscosity = computeFluid->GetViscosity();

        ImGui::Text("Gas Constant");
        if (ImGui::DragFloat("##GasConstant", &gasConstant, 0.01f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            computeFluid->SetGasConstant(gasConstant);
        }

        ImGui::Text("Rest Density");
        if (ImGui::DragFloat("##RestDensity", &restDensity, 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            computeFluid->SetRestDensity(restDensity);
        }
        ImGui::Text("Viscosity");
        if (ImGui::DragFloat("##Viscosity", &viscosity, 0.01f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            computeFluid->SetViscosity(viscosity);
        }
    }

}
