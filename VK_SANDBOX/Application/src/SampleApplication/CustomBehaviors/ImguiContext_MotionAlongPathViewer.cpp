
#include "ImguiContext_MotionAlongPathViewer.h"

#include "imgui.h"
#include "MotionAlongPathViewer.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Graphics/RenderSystems/RenderSystem.h"
#include "../../../../Engine/src/Graphics/Animation/AnimationConfig.h"

namespace IHCEngine::Component
{
    void ImguiContext_MotionAlongPathViewer::UpdateContext()
    {
        ImGui::Text("-----------------------");
        ImGui::Checkbox("debugBonesEnabled", &IHCEngine::Graphics::RenderSystem::debugBonesEnabled);
        ImGui::Checkbox("animationMeshEnabled", &IHCEngine::Graphics::RenderSystem::animationMeshEnabled);
        ImGui::Checkbox("calculateBonesWithVQS", &IHCEngine::Graphics::AnimationConfig::calculateBonesWithVQS);
        ImGui::Text("-----------------------");
        ImGui::Text("-- Key Interface --");
        ImGui::Text("' SPACE ':");
        ImGui::Text("Play animation and move along the path");
        ImGui::Text("' RIGHT ARROW ':");
        ImGui::Text("Switch Next Set of Control Points");
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
            ImGui::Text("' B ': Toggle the bone drawing on/off");
            ImGui::Text("' M ': Toggle the mesh drawing on/off");
            ImGui::TreePop();
        }
        ImGui::Text("-----------------------");
        ImGui::Text("-- Speed Info --");
    	auto scene = IHCEngine::Core::SceneManagerLocator::GetSceneManager()->GetActiveScene();
        auto gobj = scene->GetGameObjectByName("emptyGobj");
        auto viewer = gobj->GetComponent<SampleApplication::MotionAlongPathViewer>();

        float easeInTime = viewer->GetEaseInTiming();
        float easeOutTime = viewer->GetEaseOutTiming();
        float totalTime = viewer->GetTotalTime();
        ImGui::Text("EaseInTime: %.1f, EaseOutTime: %.1f", easeInTime, easeOutTime);
        ImGui::Text("TotalTime: %.1f", totalTime);
        float timePassed= viewer->GetTimePassed();
        ImGui::Text("TimePassed: %.3f", timePassed);
        if (timePassed > easeInTime && timePassed < easeOutTime)
        {
            ImGui::Text("Constant Speed");
        }
        else if (timePassed <= easeInTime)
        {
            ImGui::Text("Ease In");
        } 
        else if (timePassed >= easeInTime)
        {
            ImGui::Text("Ease Out");
        }

        ImGui::Text("-----------------------");
        ImGui::Text("-- Animation Blending Info --");
        float currentSpeed = viewer->GetCurrentSpeed();
        ImGui::Text("CurrentSpeed: %.3f", currentSpeed);
        float ubSpeed = viewer->GetUpperBoundSpeed();
        float lbSpeed = viewer->GetLowerBoundSpeed();
        ImGui::Text("UB_Speed: %.3f, LB_Speed: %.3f", ubSpeed, lbSpeed);
        float blendRatio = viewer->GetBlendRatio();
        if(blendRatio == 0.0)
        {
            ImGui::Text("Walk");
        }
        else if(blendRatio == 1.0)
        {
            ImGui::Text("Run");
        }
        else
        {
            ImGui::Text("Walk (%.3f) : Run (%.3f)", 1.0 - blendRatio, blendRatio);
        }
 
        ImGui::Text("-----------------------");
        if (ImGui::TreeNode("Control Points")) 
        {
            auto& points = viewer->GetControlPoints();

            for (size_t i = 0; i < points.size(); i++)
            {
                ImGui::Text("x: %.3f, y: %.3f, z: %.3f", points[i].x, points[i].y, points[i].z);
            }
            ImGui::TreePop();
        }

        //if (ImGui::TreeNode("Global ArcLengthTable"))
        //{
        //    auto& arclengthtable = viewer->GetArcLengthTable();
        //    ImGui::Text("totalU: %.3f", arclengthtable.totalU);
        //    ImGui::Text("totalArcLength: %.3f", arclengthtable.totalArcLength);
        //    ImGui::Text("  u  |  s = G(u)");
        //    for (size_t i = 0; i < arclengthtable.table.size(); i++)
        //    {
        //        ImGui::Text("%.3f, %.3f", arclengthtable.table[i].u, arclengthtable.table[i].arcLength);
        //    }
        //    ImGui::TreePop();
        //}

        if (ImGui::TreeNode("Normalized Global ArcLengthTable"))
        {
            auto& arclengthtable = viewer->GetArcLengthTable();
            ImGui::Text("totalU: %.3f", arclengthtable.totalU);
            ImGui::Text("totalArcLength: %.3f", arclengthtable.totalArcLength);
            ImGui::Text("  u  |  s = G(u)");
            for (size_t i = 0; i < arclengthtable.normalizedTable.size(); i++)
            {
                ImGui::Text("%.3f, %.3f", arclengthtable.normalizedTable[i].u, arclengthtable.normalizedTable[i].arcLength);
            }
            ImGui::TreePop();
        }
    }
}
