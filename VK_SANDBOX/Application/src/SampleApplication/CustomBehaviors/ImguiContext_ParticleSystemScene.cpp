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

        enableAdvection = computeParticle->GetEnableAdvection();
        enableVortex = computeParticle->GetEnableVortex();

        ImGui::Checkbox("enableAdvection", &enableAdvection);
        if (enableAdvection)
        {
            computeParticle->SetEnableAdvection(true);
        }
        else
        {
            computeParticle->SetEnableAdvection(false);
        }

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
        }
        else
        {
            computeParticle->SetEnableVortex(false);
        }

        ImGui::Text("-------------------");
        ImGui::Text("Interface");
        ImGui::Text(" WSAD : Move");
        ImGui::Text(" Mouse Right Click : Rotate Camera");

        ImGui::Text("-------------------");
    }
}
