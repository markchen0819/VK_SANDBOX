#include "AnimationViewer.h"

#include "../../../../Engine/src/Input/Input.h"
#include "../../Engine/src/Graphics/Camera.h"
#include "../../Engine/src/Core/Scene/Scene.h"
#include "../../Engine/src/Core/Scene/GameObject.h"
#include "../../Engine/src/Core/Locator/AppWindowLocator.h"
#include "../../Engine/src/Window/AppWindow.h"
#include "../../Engine/src/Core/Time/Time.h"
#include "../../../../Engine/src/Core/Scene/Components/AnimatorComponent.h"
#include "../../../../Engine/src/Graphics/RenderSystems/RenderSystem.h"

SampleApplication::AnimationViewer::AnimationViewer() {}

void SampleApplication::AnimationViewer::Awake()
{
    window = IHCEngine::Core::AppWindowLocator::GetAppWindow()->GetWindowHandle();

    // Animation Viewer Input
	camera = &(this->gameObject->GetScene()->GetCamera());
    camera->transform.Rotate(glm::vec3(-20, 0, 0));
    camera->transform.SetPosition(glm::vec3(0.0f, 10.8f, 18.5f));

    glm::vec3 cameraPosWithoutY = camera->transform.GetPosition();
    angleRespectToCenterPoint = 90;
    cameraPosWithoutY.y = 0;
    distanceToCenterPoint = length(centerPoint - cameraPosWithoutY);
    camera->LookAt(glm::vec3(0, 5, 0));

    // Model Input
    for (int i = 0; i < animationGobjs.size(); ++i)
    {
        animationGobjs[i]->SetActive(false);
    }
    animationGobjs[0]->SetActive(true);
    animationGobjs[0]->GetComponent<IHCEngine::Component::AnimatorComponent>()
        ->SetAnimation(animationSlots1[0]);

}

void SampleApplication::AnimationViewer::Start(){}
void SampleApplication::AnimationViewer::Update()
{
    HandleModelInput();
    HandleAnimationViewerInput();
}
void SampleApplication::AnimationViewer::FixedUpdate() {}
void SampleApplication::AnimationViewer::OnEnable() {}
void SampleApplication::AnimationViewer::OnDisable() {}

void SampleApplication::AnimationViewer::HandleModelInput()
{
    if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_RIGHT)) 
    {
        animationGobjs[modelIndex]->SetActive(false);
        modelIndex = (modelIndex + 1) % animationGobjs.size();
        animationGobjs[modelIndex]->SetActive(true);

        if (animationIndex == 0)
        {
            animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>()
                ->SetAnimation(animationSlots1[modelIndex]);
        }
        else
        {
            animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>()
                ->SetAnimation(animationSlots2[modelIndex]);
        }
    }
    if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_LEFT))
    {
        animationGobjs[modelIndex]->SetActive(false);
        modelIndex = (modelIndex - 1) % animationGobjs.size();
        animationGobjs[modelIndex]->SetActive(true);

        if (animationIndex == 0)
        {
            animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>()
                ->SetAnimation(animationSlots1[modelIndex]);
        }
        else
        {
            animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>()
                ->SetAnimation(animationSlots2[modelIndex]);
        }
    }
    if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_SPACE))
    {
        animationIndex = (animationIndex + 1) % 2;
        if (animationIndex == 0)
        {
            animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>()
                ->SetAnimation(animationSlots1[modelIndex]);
        }
        else
        {
            animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>()
                ->SetAnimation(animationSlots2[modelIndex]);
        }
    }

    if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_B))
    {
        // bone drawing on/off.
        IHCEngine::Graphics::RenderSystem::debugBonesEnabled = !IHCEngine::Graphics::RenderSystem::debugBonesEnabled;
    }
    if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_M))
    {
        // mesh drawing on/off.
        IHCEngine::Graphics::RenderSystem::animationMeshEnabled = !IHCEngine::Graphics::RenderSystem::animationMeshEnabled;
    }

    if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_KP_MULTIPLY))
    {
        auto ac = animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>();
        float speed = ac->GetSpeed();
        speed += 0.15;
        ac->SetSpeed(speed);
    }
    if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_KP_DIVIDE))
    {
        auto ac = animationGobjs[modelIndex]->GetComponent<IHCEngine::Component::AnimatorComponent>();
        float speed = ac->GetSpeed();
        speed -= 0.15;
        ac->SetSpeed(speed);
    }
}

void SampleApplication::AnimationViewer::HandleAnimationViewerInput()
{
    float dt = IHCEngine::Core::Time::GetDeltaTime();
    auto cameraUp = camera->transform.GetUp(); //camera->GetUp();
    auto cameraRight = camera->transform.GetRight(); //camera->GetRight();
    auto cameraForward = camera->transform.GetForward();// camera->GetForward();

    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_0)) // rotate right around target
    {
        angleRespectToCenterPoint += angleSpeed * dt;
        glm::vec3 cameraPos = camera->transform.GetPosition();
        glm::vec3 newPos;
        newPos.x = centerPoint.x + distanceToCenterPoint * cos(glm::radians(angleRespectToCenterPoint));
        newPos.y = cameraPos.y;  // Y remains the same as we're not moving vertically
        newPos.z = centerPoint.z + distanceToCenterPoint * sin(glm::radians(angleRespectToCenterPoint));
        camera->transform.SetWorldPosition(newPos);
        camera->LookAt(glm::vec3(0, 5, 0));
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_DECIMAL))// rotate left around target
    {
        angleRespectToCenterPoint -= angleSpeed * dt;
        glm::vec3 cameraPos = camera->transform.GetPosition();
        glm::vec3 newPos;
        newPos.x = centerPoint.x + distanceToCenterPoint * cos(glm::radians(angleRespectToCenterPoint));
        newPos.y = cameraPos.y;  // Y remains the same as we're not moving vertically
        newPos.z = centerPoint.z + distanceToCenterPoint * sin(glm::radians(angleRespectToCenterPoint));
        camera->transform.SetWorldPosition(newPos);
        camera->LookAt(glm::vec3(0, 5, 0));
    }
    if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_KP_5)) // reset
    {
        angleRespectToCenterPoint = 90;
        glm::vec3 cameraPos = camera->transform.GetPosition();
        glm::vec3 newPos;
        newPos.x = centerPoint.x + distanceToCenterPoint * cos(glm::radians(angleRespectToCenterPoint));
        newPos.y = cameraPos.y;  // Y remains the same as we're not moving vertically
        newPos.z = centerPoint.z + distanceToCenterPoint * sin(glm::radians(angleRespectToCenterPoint));
        camera->transform.SetWorldPosition(newPos);
        camera->LookAt(glm::vec3(0, 5, 0));
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_4)) // left
    {
        auto p = -cameraRight * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_6)) // right
    {
        auto p = cameraRight * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_8)) // up
    {
        auto p = cameraUp * movementSpeed * dt;
        camera->transform.Translate(p);
        auto cameraX = camera->transform.GetPosition().x;
        camera->LookAt(glm::vec3(cameraX, 5, 0));
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_2)) // down
    {
        auto p = -cameraUp * movementSpeed * dt;
        camera->transform.Translate(p);
        auto cameraX = camera->transform.GetPosition().x;
        camera->LookAt(glm::vec3(cameraX, 5, 0));
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_ADD)) // zoom in
    {
        float newFov = camera->GetFOV() - 1 * zoomSpeed;
        camera->SetFOV(newFov);
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_KP_SUBTRACT)) // zoom in
    {
        float newFov = camera->GetFOV() + 1 * zoomSpeed;
        camera->SetFOV(newFov);
    }
}

