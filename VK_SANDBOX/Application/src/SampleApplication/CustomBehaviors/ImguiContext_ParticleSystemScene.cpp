#include "ImguiContext_ParticleSystemScene.h"
#include "imgui.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Core/Scene/Components/ComputeParticleComponent.h"
#include "../../../../Engine/src/Graphics/RenderSystems/RenderSystem.h"
#include "../../../../Engine/src/Graphics/Animation/AnimationConfig.h"

namespace IHCEngine::Component
{
    void ImguiContext_ParticleSystemScene::UpdateContext()
    {
        auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
        auto scene = sceneManager->GetActiveScene();
        auto camera = scene->GetGameObjectByName("camera");
        auto gobj = scene->GetGameObjectByName("particleGobj");
        auto computeParticle = gobj->GetComponent<ComputeParticleComponent>();

        ImGui::Text("------------------------");
        ImGui::Text(" Interface");
        ImGui::Text(" WSAD/ Shift/ Control : Move Camera");
        ImGui::Text(" Mouse Right Click Drag: Rotate Camera");
        ImGui::Text(" Mouse Scroll: Zoom In/Out");
        ImGui::Text(" F5: Full scene Reset");
        ImGui::Text("------------------------");

        enableAdvection = computeParticle->GetEnableAdvection();
        enableVortex = computeParticle->GetEnableVortex();
        enableGravity = computeParticle->GetEnableGravity();
        enableBounce = computeParticle->GetEnableBounce();
        enableSpiral = computeParticle->GetEnableSpiral();

        ImGui::Checkbox("enableAdvection", &enableAdvection);
        if (enableAdvection)
        {
            ImGui::Text("#############");
            ImGui::Text("Description: ");
            ImGui::Text("Advection creates a flow");
            ImGui::Text("The closer to the flow axis runs faster");
            ImGui::Text("The farther to the flow axis runs slower");
            ImGui::Text("#############");

            computeParticle->SetEnableAdvection(true);
            float flowMaxVelocity = computeParticle->GetFlowMaxVelocity();
            float flowWidth = computeParticle->GetFlowWidth();
            glm::vec4 flowDirection = computeParticle->GetFlowDirection();
            glm::vec4 flowCenter = computeParticle->GetFlowCenter();
            ImGui::Text("Flow Direction");
            if (ImGui::DragFloat4("##FlowDirection", &flowDirection[0], 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
            {
                computeParticle->SetFlowDirection(flowDirection);
            }
            ImGui::Text("Flow Center");
            if (ImGui::DragFloat4("##FlowCenter", &flowCenter[0], 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
            {
                computeParticle->SetFlowCenter(flowCenter);
            }

            ImGui::Text("FlowMaxVelocity");
            ImGui::SliderFloat("##FlowMaxVelocity", &flowMaxVelocity, 0.0f, 50.0f);
            ImGui::Text("FlowWidth");
            ImGui::SliderFloat("##FlowWidth", &flowWidth, 0.0f, 100.0f);
            computeParticle->SetFlowMaxVelocity(flowMaxVelocity);
            computeParticle->SetFlowWidth(flowWidth);
        }
        else
        {
            computeParticle->SetEnableAdvection(false);
        }
        ImGui::Text("-------------------");
        ImGui::Checkbox("enableVortex", &enableVortex);
        if (enableVortex)
        {
            ImGui::Text("#############");
            ImGui::Text("Description: ");
            ImGui::Text("Kappa controls Vortex rotation speed");
            ImGui::Text("Tau controls Vortex center strength");
            ImGui::Text("#############");
            computeParticle->SetEnableVortex(true);
            float kappa = computeParticle->GetRotationRate();
            float tau = computeParticle->GetTightness();
            ImGui::Text("Rotation Rate (kappa)");
            ImGui::SliderFloat("##RotationRate", &kappa, -20.0f, 20.0f);
            ImGui::Text("Tightness (tau)");
            ImGui::SliderFloat("##Tightness", &tau, 0.0f, 10.0f);
            computeParticle->SetRotationRate(kappa);
            computeParticle->SetTightness(tau);

            glm::vec4 vortexCenter = computeParticle->GetVortexCenter();
            glm::vec4 vortexAxis = computeParticle->GetVortexAxis();
            ImGui::Text("Vortex Axis");
            if(ImGui::DragFloat4("##VortexAxis", &vortexAxis[0], 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
            {
                computeParticle->SetVortexAxis(vortexAxis);
            }
            ImGui::Text("Vortex Center");
            if (ImGui::DragFloat4("##VortexCenter", &vortexCenter[0], 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
            {
                computeParticle->SetVortexCenter(vortexCenter);
            }
        }
        else
        {
            computeParticle->SetEnableVortex(false);
        }
        ImGui::Text("-------------------");
        ImGui::Checkbox("enableGravity", &enableGravity);
        if (enableGravity)
        {
            ImGui::Text("#############");
            ImGui::Text("Description: ");
            ImGui::Text("Gravity creates force base on the vec4 direction");
            ImGui::Text("#############");
            computeParticle->SetEnableGravity(true);
            glm::vec4 gravity = computeParticle->GetGravity();
            ImGui::Text("Gravity");
            if (ImGui::DragFloat4("##Gravity", &gravity[0], 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
            {
                computeParticle->SetGravity(gravity);
            }
        }
        else
        {
            computeParticle->SetEnableGravity(false);
        }
        ImGui::Text("-------------------");
        ImGui::Checkbox("enableBounce", &enableBounce);
        if (enableBounce)
        {
            ImGui::Text("#############");
            ImGui::Text("Description: ");
            ImGui::Text("Bounce happens on the XZ - plane");
            ImGui::Text("The y-component of velocity will be reversed");
            ImGui::Text("Energy conservations is based on restitution");
            ImGui::Text("#############");
            computeParticle->SetEnableBounce(true);
            float restitution = computeParticle->GetRestitution();
            ImGui::Text("Restitution");
            ImGui::SliderFloat("##Restitution", &restitution, 0.0f, 2.0f);
            computeParticle->SetRestitution(restitution);
        }
        else
        {
            computeParticle->SetEnableBounce(false);
        }
        ImGui::Text("-------------------");
        ImGui::Checkbox("enableSpiral", &enableSpiral);
        if (enableSpiral)
        {
            computeParticle->SetEnableSpiral(true);

            ImGui::Text("#############");
            ImGui::Text("Description: ");
            ImGui::Text("Creates a spiral along the up vector");
            ImGui::Text("Global Axis makes particles spiral as a whole");
            ImGui::Text("Local Axis makes particles self rotate with angular speed (sigma)");
            ImGui::Text("#############");

            spiralWithGlobalAxis = computeParticle->GetSpiralWithGlobalAxis();
            ImGui::Checkbox("Spiral With Global Axis", &spiralWithGlobalAxis);
            if(spiralWithGlobalAxis)
            {
                float spiralRadius = computeParticle->GetSpiralRadius();
                float spiralAngularSpeed = computeParticle->GetSpiralAngularSpeed();
                float sprialAxisSpeed = computeParticle->GetSpiralAxisSpeed();
                ImGui::Text("Spiral Radius");
                ImGui::SliderFloat("##SpiralRadius", &spiralRadius, 0.0f, 20.0f);
                computeParticle->SetSpiralRadius(spiralRadius);
                ImGui::Text("Spiral Angular Speed");
                ImGui::SliderFloat("##SpiralAngularSpeed", &spiralAngularSpeed, -5.0f, 5.0f);
                computeParticle->SetSpiralAngularSpeed(spiralAngularSpeed);
                ImGui::Text("Spiral Axis Speed");
                ImGui::SliderFloat("##SpiralAxisSpeed", &sprialAxisSpeed, -20.0f, 20.0f);
                computeParticle->SetSpiralAxisSpeed(sprialAxisSpeed);
            }
            else
            {
                float spiralAngularSpeed = computeParticle->GetSpiralAngularSpeed();
                ImGui::Text("Spiral Angular Speed (Sigma)");
                ImGui::SliderFloat("##SpiralAngularSpeed", &spiralAngularSpeed, -20.0f, 20.0f);
                computeParticle->SetSpiralAngularSpeed(spiralAngularSpeed);
            }
            computeParticle->SetSpiralWithGlobalAxis(spiralWithGlobalAxis);
        }
        else
        {
            computeParticle->SetEnableSpiral(false);
        }
        ImGui::Text("-------------------");

        if (ImGui::Button("Example 1: Advection + Vortex + Gravity + Bounce"))
        {
            Reset();
            computeParticle->SetEnableAdvection(true);
            computeParticle->SetEnableVortex(true);
            computeParticle->SetEnableGravity(true);
            computeParticle->SetEnableBounce(true);

            computeParticle->SetFlowDirection(glm::vec4(0.7f, 0.7f, 0, 0));
            computeParticle->SetFlowMaxVelocity(34.0f);
            computeParticle->SetFlowWidth(16.0f);

            computeParticle->SetRotationRate(20.0f);
            computeParticle->SetTightness(1.228f);
            computeParticle->SetVortexCenter(glm::vec4(0, 0, -15, 0));
            computeParticle->SetVortexAxis(glm::vec4(0, 1, 0, 0));
        }
        if (ImGui::Button("Example 2: Advection + Gravity + Bounce")) 
        {
            Reset();
            computeParticle->SetEnableGravity(true);
            computeParticle->SetEnableBounce(true);
            computeParticle->SetEnableAdvection(true);
            computeParticle->SetFlowDirection(glm::vec4(1,0,0,0));
            computeParticle->SetFlowCenter(glm::vec4(0, 2, 0, 0));
            computeParticle->SetFlowMaxVelocity(9.0f);
            computeParticle->SetFlowWidth(17.0f);
        }
        if (ImGui::Button("Example 3: Vortex + Spiral"))
        {
            Reset();
            computeParticle->SetEnableVortex(true);
            computeParticle->SetEnableSpiral(true);

            computeParticle->SetRotationRate(14.615f);
            computeParticle->SetTightness(1.558f);
            computeParticle->SetVortexCenter(glm::vec4(6.4f, 6.1f, 24.3f, 0));
            computeParticle->SetVortexAxis(glm::vec4(0, 0, 1, 0));

            computeParticle->SetSpiralRadius(10.48f);
            computeParticle->SetSpiralAngularSpeed(1.279f);
            computeParticle->SetSpiralAxisSpeed(8.077f);
        }

    }

    void ImguiContext_ParticleSystemScene::Reset()
    {
        auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
        auto scene = sceneManager->GetActiveScene();
        auto gobj = scene->GetGameObjectByName("particleGobj");
        auto computeParticle = gobj->GetComponent<ComputeParticleComponent>();

        computeParticle->SetEnableAdvection(false);
        computeParticle->SetEnableVortex(false);
        computeParticle->SetEnableGravity(false);
        computeParticle->SetEnableBounce(false);
        computeParticle->SetEnableSpiral(false);
    }
}
