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

        ImGui::Text("-------------------");
        ImGui::Text("Interface");
        ImGui::Text(" WSAD/ Shift/ Control : Move Camera");
        ImGui::Text(" Mouse Right Click Drag: Rotate Camera");
        ImGui::Text("-------------------");

        enableAdvection = computeParticle->GetEnableAdvection();
        enableVortex = computeParticle->GetEnableVortex();
        enableGravity = computeParticle->GetEnableGravity();
        enableBounce = computeParticle->GetEnableBounce();

        ImGui::Checkbox("enableAdvection", &enableAdvection);
        if (enableAdvection)
        {
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

            ImGui::Text("Adevection creates a flow");
            ImGui::Text("The closer to the flow axis runs faster");
            ImGui::Text("The farther to the flow axis runs slower");
        }
        else
        {
            computeParticle->SetEnableAdvection(false);
        }
        ImGui::Text("-------------------");
        ImGui::Checkbox("enableVortex", &enableVortex);
        if (enableVortex)
        {
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

    }
}
