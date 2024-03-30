#include "ImguiContext_GrassScene.h"
#include "imgui.h"
#include "../../../../../Engine/src/Core/Scene/Components/ComputeGrassComponent.h"
#include "../../../../../Engine/src/Graphics/VKWraps/IHCTexture.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Application/src/SampleApplication/Scene/GrassScene.h";
#include "../CPUFrustumCulling/CPUFrustumCulling.h"

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
            ImGui::Text("Wind is created using a simple noise texture scrolling");
            ImGui::Text("User can self define the wind direction and strength");
            ImGui::Text("The up and down motion of the tip of the grass blade is a sin wave");
            ImGui::Text("User can self define the sway strength and frequency");
            ImGui::Text("------------------------");
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
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.windDirection = glm::normalize(windDirection);
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
            ImGui::Text("The curvature of the grass blade is influenced by a cubic bezier curve");
            ImGui::Text("global tilt: tilts all the grass blades in the same angle");
            ImGui::Text("bend: a simple parameter that pulls the controls points of the curve away");
            ImGui::Text("controlPt: instead of bend, user can use this for more control over the curve");
            ImGui::Text("------------------------");
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
            ImGui::Text("All grass blades are rotated the same direction using this parameter");
            ImGui::Text("The normal of the grass blade is calculated using the derivative of the curve");
            ImGui::Text("Using this we can see how a basic lighting model interacts with different rotation angles");
            ImGui::Text("------------------------");

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

        if (ImGui::CollapsingHeader("Debug (CPU Frustum Culling, LOD)"))
        {
            ImGui::Text("showWorldNormals: this shows the normals on the curve in colors");
            ImGui::Text("dropFrustum: frustum culling is done on the CPU with AABB bounding boxes");
            ImGui::Text("using this we can see the culled parts");
            ImGui::Text("showLOD: LOD is done on the CPU, further chunks are swapped using a lower LOD model");
            ImGui::Text("------------------------");

            auto cpuFrustumCullingGobj = scene->GetGameObjectByName("frustumCullingGobj");
            auto cpuFrustumCulling = cpuFrustumCullingGobj->GetComponent<SampleApplication::CPUFrustumCulling>();

            showWorldNormals = static_cast<bool>(computeParticle->grassBladePropertyOverride.showWorldNormals);
            showLOD = static_cast<bool>(computeParticle->grassBladePropertyOverride.showLOD);
            dropFrustum = cpuFrustumCulling->GetIsFrustumDropped();

            ImGui::Checkbox("showWorldNormals", &showWorldNormals);
            if (showWorldNormals)
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.showWorldNormals = 1;
                }
            }
            else
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.showWorldNormals = 0;
                }
            }

            ImGui::Checkbox("showLOD", &showLOD);
            if (showLOD)
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.showLOD = 1;
                }
            }
            else
            {
                for (auto gobj : grassChunkGobjs)
                {
                    gobj->GetComponent<ComputeGrassComponent>()->grassBladePropertyOverride.showLOD = 0;
                }
            }

            ImGui::Checkbox("dropFrustum", &dropFrustum);
            if (dropFrustum)
            {
                cpuFrustumCulling->DropFrustum(true);
            }
            else
            {
                cpuFrustumCulling->DropFrustum(false);
            }
        }
    }

}
