#include "ImguiContext_PhysicallyBasedSimulationViewer.h"

#include "imgui.h"
#include "PhysicallyBasedSimulationViewer.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Graphics/RenderSystems/RenderSystem.h"
#include "../../../../Engine/src/Physics/SoftBody.h"


namespace IHCEngine::Component
{
    void ImguiContext_PhysicallyBasedSimulationViewer::UpdateContext()
    {
        auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
        ImGui::Text("-----------------------");
        if (ImGui::Checkbox("reloadScene", &reloadScene))
        {
            sceneManager->SetNextSceneToLoad(3);
        }
        ImGui::Text("-----------------------");
        ImGui::Checkbox("wireframeEnabled", &IHCEngine::Graphics::RenderSystem::wireframeEnabled);
    	ImGui::Text("-----------------------");

        auto scene = sceneManager->GetActiveScene();
        auto camera = scene->GetGameObjectByName("camera");
        auto viewer = camera->GetComponent<SampleApplication::PhysicallyBasedSimulationViewer>();

        startedSimulating = viewer->HasStartedSimulating();
        if(!startedSimulating)
        {
            ImGui::Text("Simulation starts in 3 seconds");
            ImGui::Text("Please wait until frameRate is stable after scene load");
            return;
        }

        ImGui::Text("Integration Method");
        currentItem = static_cast<int>(viewer->GetCloth().GetIntegrationMethod());
        if (ImGui::BeginCombo("##Integration Method", integrationMethodNames[currentItem]))
        {
            for (int i = 0; i < 4; i++) {
                bool isSelected = (currentItem == i);
                if (ImGui::Selectable(integrationMethodNames[i], isSelected)) 
                {
                    currentItem = i;
                    viewer->GetCloth().SetIntegrationMethod(static_cast<Physics::IntegrationMethod>(i));
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        isP1Pinned = viewer->IsP1Pinned();
        isP2Pinned = viewer->IsP2Pinned();
        isP3Pinned = viewer->IsP3Pinned();
        isP4Pinned = viewer->IsP4Pinned();
        if(ImGui::Checkbox("isP1Pinned", &isP1Pinned))
        {
            viewer->SetPinned(1);
        }
        if (ImGui::Checkbox("isP2Pinned", &isP2Pinned))
        {
            viewer->SetPinned(2);
        }
        if (ImGui::Checkbox("isP3Pinned", &isP3Pinned))
        {
            viewer->SetPinned(3);
        }
        if (ImGui::Checkbox("isP4Pinned", &isP4Pinned))
        {
            viewer->SetPinned(4);
        }


        ImGui::Text("Wind direction is ");
        ImGui::Text("from sphere position to cloth center");
        if (ImGui::SliderFloat("WindStrength", &windstrength, 0.0f, 5.0f))
        {
            viewer->SetWindStrength(windstrength);
        }

    }
}
