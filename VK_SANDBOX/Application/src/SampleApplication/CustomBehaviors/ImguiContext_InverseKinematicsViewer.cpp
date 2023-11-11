#include "ImguiContext_InverseKinematicsViewer.h"


#include "imgui.h"
#include "InverseKinematicsViewer.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Core/Scene/Components/IKComponent.h"
#include "../../../../Engine/src/Graphics/RenderSystems/RenderSystem.h"
#include "../../../../Engine/src/Graphics/Animation/AnimationConfig.h"

namespace IHCEngine::Component
{

    void ImguiContext_InverseKinematicsViewer::UpdateContext()
    {

        ImGui::Checkbox("debugBonesEnabled", &IHCEngine::Graphics::RenderSystem::debugBonesEnabled);
        ImGui::Checkbox("animationMeshEnabled", &IHCEngine::Graphics::RenderSystem::animationMeshEnabled);
        ImGui::Text("-----------------------");

        auto scene = IHCEngine::Core::SceneManagerLocator::GetSceneManager()->GetActiveScene();
        auto ikGobj = scene->GetGameObjectByName("IKGobj");
        auto ikComponent = ikGobj->GetComponent<IKComponent>();
        auto camera = scene->GetGameObjectByName("camera");
        auto viewer = camera->GetComponent<SampleApplication::InverseKinematicsViewer>();

        ImGui::Text("IK Mode");
        if (ImGui::BeginCombo("##Root", ikMode[selectedMode]))
        {
            for (int i = 0; i < 2; i++)
            {
                bool isSelected = (selectedMode == i);
                if (ImGui::Selectable(ikMode[i], isSelected))
                {
                    selectedMode = i;
                    std::string modeStr = ikMode[selectedMode];
                    viewer->SetMode(modeStr);
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }

            }
            ImGui::EndCombo();
        }

        ImGui::Text("-----------------------");

        ImGui::Text("IK Chain:");
        ImGui::Text("-> mixamorig:RightShoulder (Root)");
        ImGui::Text("-> mixamorig:RightArm");
        ImGui::Text("-> mixamorig:RightForeArm");
        ImGui::Text("-> mixamorig:RightHand");
        ImGui::Text("-> mixamorig:RightHandIndex1");
        ImGui::Text("-> mixamorig:RightHandIndex2");
        ImGui::Text("-> mixamorig:RightHandIndex3");
        ImGui::Text("-> mixamorig:RightHandIndex4");
        ImGui::Text("-> mixamorig:RightHandIndex4_end (E.E.)");

        ImGui::Text("-----------------------");

        ImGui::Text("State: ");
        if(selectedMode==1) //IK_Realtime
        {
            ImGui::Text("Free control");
        }
        else
        {
            if (!viewer->IsAnimating())
            {
                ImGui::Text("Animating, target object not controllable");
            }
            else
            {
                ImGui::Text("Animation Done");
            }
        }


        //if (ImGui::BeginCombo("##Root", ikChain[selectedRoot]))
        //{
        //    for (int i = 0; i < 12; i++)
        //    {
        //        bool isSelected = (selectedRoot == i);
        //        if (ImGui::Selectable(ikChain[i], isSelected))
        //        {
        //            selectedRoot = i;
        //            ikComponent->SetRootAndEE(ikChain[selectedRoot], ikChain[selectedEndEffector]);

        //        }
        //        if (isSelected)
        //        {
        //            ImGui::SetItemDefaultFocus();
        //        }
        //          
        //    }
        //    ImGui::EndCombo();
        //}

        //if (ImGui::BeginCombo("## End Effector", ikChain[selectedEndEffector]))
        //{
        //    for (int i = 0; i < 12; i++)
        //    {
        //        bool isSelected = (selectedEndEffector == i);
        //        if (ImGui::Selectable(ikChain[i], isSelected))
        //        {
        //            selectedEndEffector = i;
        //            ikComponent->SetRootAndEE(ikChain[selectedRoot], ikChain[selectedEndEffector]);

        //        }
        //        if (isSelected)
        //        {
        //            ImGui::SetItemDefaultFocus();
        //        }

        //    }
        //    ImGui::EndCombo();
        //}

    }
}
