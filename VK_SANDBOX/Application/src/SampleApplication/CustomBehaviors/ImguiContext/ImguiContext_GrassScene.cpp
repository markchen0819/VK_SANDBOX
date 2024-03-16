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
            swayStrength = computeParticle->grassBladePropertyOverride.swayStrength;
            swayFrequency = computeParticle->grassBladePropertyOverride.swayFrequency;

            ImGui::Text("Wind Speed (NoiseTexture Scrolling)");
            if (ImGui::DragFloat("##Wind Speed ", &windSpeed, 0.01f, 0.0, 1.0, "%.3f"))
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
            ImGui::Text("Sway Strength (blade swaying up and down on global y axis)");
            if (ImGui::DragFloat("##Sway Strength", &swayStrength, 0.01f, 0.0f, 1.0f, " % .3f"))
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.swayStrength = swayStrength;
                }
            }
            ImGui::Text("Sway Frequency");
            if (ImGui::DragFloat("##Sway Frequency", &swayFrequency, 0.1f, 0.0f, FLT_MAX, " % .3f"))
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.swayFrequency = swayFrequency;
                }
            }
        }

        if (ImGui::CollapsingHeader("Curvature of Blade"))
        {
            enableGlobalTilt = computeParticle->grassBladePropertyOverride.enableGlobalTilt;
            globalTilt = computeParticle->grassBladePropertyOverride.globalTilt;
            controlPtA = computeParticle->grassBladePropertyOverride.controlPtA;
            controlPtB = computeParticle->grassBladePropertyOverride.controlPtB;
            enableControlPt = computeParticle->grassBladePropertyOverride.enableControlPt;
            bend = computeParticle->grassBladePropertyOverride.bend;

            ImGui::Checkbox("enableGlobalTilt", &enableGlobalTilt);
            if (enableGlobalTilt)
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.enableGlobalTilt = true;
                }
                ImGui::Text("Tilt");
                if (ImGui::DragFloat("##Tilt", &globalTilt, 0.1f, 0.0f, FLT_MAX, "%.3f"))
                {
                    for (auto gobj : grassChunkGobjs)
                    {
                        gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.globalTilt = globalTilt;
                    }
                }
            }
            else
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.enableGlobalTilt = false;
                }
            }

            ImGui::Checkbox("enableControlPt", &enableControlPt);
            if (enableControlPt)
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.enableControlPt = true;
                }
                ImGui::Text("Starting from the base of grass blade is P0 - PtA - PtB - P1");
                ImGui::Text("Modify the x and y component to adjust the curvature of the Bezier Curve");
                ImGui::Text("ControlPtA");
                if (ImGui::DragFloat4("##ControlPtA", &controlPtA[0], 0.01f, -FLT_MAX, FLT_MAX, "%.3f"))
                {
                    for (auto gobj : grassChunkGobjs)
                    {
                        gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.controlPtA = controlPtA;
                    }
                }
                ImGui::Text("ControlPtB");
                if (ImGui::DragFloat4("##ControlPtB", &controlPtB[0], 0.01f, -FLT_MAX, FLT_MAX, "%.3f"))
                {
                    for (auto gobj : grassChunkGobjs)
                    {
                        gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.controlPtB = controlPtB;
                    }
                }
            }
            else
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.enableControlPt = false;
                }
                ImGui::Text("Starting from the base of grass blade is P0 - PointA - PointA - P1");
                ImGui::Text("Bend controls how far PointA moves away from the vector P0P1");
                ImGui::Text("Bend");
                if (ImGui::DragFloat("##Bend", &bend, 0.01f, -FLT_MAX, FLT_MAX, "%.3f"))
                {
                    for (auto gobj : grassChunkGobjs)
                    {
                        gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.bend = bend;
                    }
                }
            }



        }

        if (ImGui::CollapsingHeader("Override Rotation"))
        {
            overrideRotation = computeParticle->grassBladePropertyOverride.enableRotationOverride;
            rotation = computeParticle->grassBladePropertyOverride.globalRotation;

            ImGui::Checkbox("overrideRotation", &overrideRotation);
            if (overrideRotation)
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.enableRotationOverride = true;
                }
                ImGui::Text("Amount Of Rotation");
                if (ImGui::DragFloat("##Amount Of Rotation", &amountOfRotation, 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
                {
                    float rotationAngle = glm::radians(amountOfRotation);
                    glm::vec3 rotationAxis = glm::vec3(0, 1, 0);
                    glm::quat randomRotation = glm::angleAxis(rotationAngle, rotationAxis);
                    glm::vec4 globalRotation = glm::vec4(randomRotation.w, randomRotation.x, randomRotation.y, randomRotation.z);

                	for (auto gobj : grassChunkGobjs)
                    {
                        gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.globalRotation = globalRotation;
                    }
                }
            }
            else
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.enableRotationOverride = false;
                }
            }
        }

    }

}
