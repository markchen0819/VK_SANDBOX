#include "ImguiContext_GrassScene.h"
#include "imgui.h"
#include "../../../../../Engine/src/Core/Scene/Components/ComputeGrassComponent.h"
#include "../../../../../Engine/src/Graphics/VKWraps/IHCTexture.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Application/src/SampleApplication/Scene/GrassScene.h";

namespace IHCEngine::Component
{
    void ImguiContext_GrassScene::UpdateContext()
    {
        auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
        auto scene = sceneManager->GetActiveScene();
        auto grassScene = dynamic_cast<SampleApplication::GrassScene*>(scene);
        auto grassChunkGobjs = grassScene->GetGrassChunkGobjs();
        auto camera = scene->GetGameObjectByName("camera");

        ImGui::Text("------------------------");
        ImGui::Text(" Interface");
        ImGui::Text(" WSAD/ Shift/ Control : Move Camera");
        ImGui::Text(" Mouse Right Click Drag: Rotate Camera");
        ImGui::Text(" Mouse Scroll: Zoom In/Out");
        ImGui::Text(" F6: Full scene Reset");
        ImGui::Text("------------------------");

        auto computeParticle = grassChunkGobjs[0]->GetComponent<ComputeGrassComponent>();

        if (ImGui::CollapsingHeader("Wind"))
        {
            windDirection = computeParticle->grassBladePropertyOverride.windDirection;
            windStrength = computeParticle->grassBladePropertyOverride.windStrength;
            windSpeed = computeParticle->grassBladePropertyOverride.windSpeed;
            ImGui::Text("Wind Speed (NoiseTexture Scrolling)");
            if (ImGui::DragFloat("##Wind Speed ", &windSpeed, 0.1f, 0.0, 5.0, "%.3f"))
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.windSpeed = windSpeed;
                }
            }
            ImGui::Text("Wind Direction");
            if (ImGui::DragFloat4("##Wind Direction", &windDirection[0], 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.windDirection = windDirection;
                }
            }
            ImGui::Text("Wind Strength");
            if (ImGui::DragFloat("##Wind Strength", &windStrength, 0.1f, 0.0f, FLT_MAX, " % .3f"))
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.windStrength = windStrength;
                }
            }

        }


    }

}
