#include "ImguiContext_AnimationViewer.h"

#include "imgui.h"
#include "../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../Engine/src/Graphics/RenderSystems/RenderSystem.h"
#include "../../../../Engine/src/Graphics/Animation/AnimationConfig.h"

namespace IHCEngine::Component
{
    void ImguiContext_AnimationViewer::UpdateContext()
    {
        auto cam = IHCEngine::Core::SceneManagerLocator::GetSceneManager()->GetActiveScene()->GetCamera();
        //glm::vec3 cameraPos = cam.transform.GetPosition();
        //glm::vec3 cameraRot = cam.transform.GetRotation();
        //ImGui::InputFloat3("CameraPosition", &cameraPos[0]);
        //ImGui::InputFloat3("CameraRotation", &cameraRot[0]);

        ImGui::Checkbox("wireframeEnabled", &IHCEngine::Graphics::RenderSystem::wireframeEnabled);
        ImGui::Checkbox("debugBonesEnabled", &IHCEngine::Graphics::RenderSystem::debugBonesEnabled);
        ImGui::Checkbox("animationMeshEnabled", &IHCEngine::Graphics::RenderSystem::animationMeshEnabled);
        ImGui::Checkbox("calculateBonesWithVQS", &IHCEngine::Graphics::AnimationConfig::calculateBonesWithVQS);

        ImGui::Text("-------------------");
        ImGui::Text("Interface");
        ImGui::Text("' SPACE ': Cycle to the next animations");
        ImGui::Text("' RIGHT/LEFT ARROW ': Switch Model");

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

        ImGui::Text("' * ': Increase the animation rate");
        ImGui::Text("' / ': Decrease the animation rate");
        ImGui::Text("' B ': Toggle the bone drawing on/off");
        ImGui::Text("' M ': Toggle the mesh drawing on/off");
        ImGui::Text("-------------------");
    }
}
