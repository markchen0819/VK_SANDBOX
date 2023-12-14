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

        ImGui::Checkbox("enabledWind", &enableWind);
        if (enableWind)
        {
            ImGui::Text("Wind direction: ");
            ImGui::Text("From sphere position to cloth center");
            ImGui::Text("WindStrength: ");
            ImGui::SliderFloat("##WindStrength", &windStrength, 0.0f, 5.0f);
            viewer->SetWindStrength(windStrength);
        }
        else
        {
            viewer->SetWindStrength(0.0);
        }

        ImGui::Text("-----------------------");
        ImGui::Text("-- Key Interface --");
        ImGui::Text("' B ':");
        ImGui::Text(" Toggle wireframe drawing on/off");

        ImGui::Text("' W ':");
        ImGui::Text("Move sphere on global -z axis");
        ImGui::Text("' S ':");
        ImGui::Text("Move sphere on global z axis");
        ImGui::Text("' A ':");
        ImGui::Text("Move sphere on global -x axis");
        ImGui::Text("' D ':");
        ImGui::Text("Move sphere on global x axis");
        ImGui::Text("' E ':");
        ImGui::Text("Move sphere on global y axis");
        ImGui::Text("' Q ':");
        ImGui::Text("Move sphere on global -y axis");

        ImGui::Text("-----------------------");

        if (ImGui::TreeNode("Other Interface"))
        {
            ImGui::Text("' 0 ': Rotate camera left about camera center");
            ImGui::Text("' . ': Rotate camera right about camera center");
            ImGui::Text("' 5 ': Reset camera to default position");
            ImGui::Text("' 4 ': Move camera AND camera center along the camera's left vector");
            ImGui::Text("' 6 ': Move camera AND camera center along the camera's right vector");
            ImGui::Text("' 8 ': Move camera but NOT center along the camera's up vector");
            ImGui::Text("' 8 ': Move camera but NOT center along the camera's up vector");
            ImGui::Text("' 2 ': Move camera but NOT center along the camera's down vector");
            ImGui::Text("' + ': Zoom camera in towards camera center");
            ImGui::Text("' - ': Zoom camera out from camera center");
            ImGui::TreePop();
        }
        ImGui::Text("-----------------------");

    }
}
