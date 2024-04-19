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
        radius = computeFluid->GetRadius();
        maxBound = computeFluid->GetMaxBound();
        minBound = computeFluid->GetMinBound();

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
        if (ImGui::DragFloat("##Viscosity", &viscosity, 0.001f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            computeFluid->SetViscosity(viscosity);
        }
        ImGui::Text("Density Radius");
        if (ImGui::DragFloat("##DensityRadius", &radius, 0.01f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            computeFluid->SetRadius(radius);
        }
        ImGui::Text("Max Bound");
        if (ImGui::DragFloat4("##Max Bound", &maxBound[0], 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            computeFluid->SetMaxBound(maxBound);
        }
        ImGui::Text("Min Bound");
        if (ImGui::DragFloat4("##Min Bound", &minBound[0], 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            computeFluid->SetMinBound(minBound);
        }
        ImGui::Text("Move Interaction Sphere: ");
        ImGui::Text(" IKJL/ U/ O");
    }

}
