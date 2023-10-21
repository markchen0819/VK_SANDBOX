
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
        ImGui::Checkbox("debugBonesEnabled", &IHCEngine::Graphics::RenderSystem::debugBonesEnabled);
        ImGui::Checkbox("animationMeshEnabled", &IHCEngine::Graphics::RenderSystem::animationMeshEnabled);
        ImGui::Checkbox("calculateBonesWithVQS", &IHCEngine::Graphics::AnimationConfig::calculateBonesWithVQS);

    	auto scene = IHCEngine::Core::SceneManagerLocator::GetSceneManager()->GetActiveScene();
        auto gobj = scene->GetGameObjectByName("emptyGobj");
        auto viewer = gobj->GetComponent<SampleApplication::MotionAlongPathViewer>();

        float currentSpeed = viewer->GetCurrentSpeed();
        float timePassed= viewer->GetTimePassed();
        ImGui::Text("CurrentSpeed: %.3f", currentSpeed);
        ImGui::Text("TimePassed: %.3f", timePassed);

        if (ImGui::TreeNode("Control Points")) 
        {
            auto& points = viewer->GetControlPoints();

            for (size_t i = 0; i < points.size(); i++)
            {
                ImGui::Text("x: %.3f, y: %.3f, z: %.3f", points[i].x, points[i].y, points[i].z);
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Global ArcLengthTable"))
        {
            auto& arclengthtable = viewer->GetArcLengthTable();
            ImGui::Text("totalU: %.3f", arclengthtable.totalU);
            ImGui::Text("totalArcLength: %.3f", arclengthtable.totalArcLength);
            ImGui::Text("  u  |  s = G(u)");
            for (size_t i = 0; i < arclengthtable.table.size(); i++)
            {
                ImGui::Text("%.3f, %.3f", arclengthtable.table[i].u, arclengthtable.table[i].arcLength);
            }
            ImGui::TreePop();
        }

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
