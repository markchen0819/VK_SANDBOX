#include <glm/gtx/string_cast.hpp> 
#include "CameraController.h"

#include "../../../../Engine/src/Input/Input.h"
#include "../../Engine/src/Graphics/Camera.h"
#include "../../Engine/src/Core/Scene/Scene.h"
#include "../../Engine/src/Core/Scene/GameObject.h"
#include "../../Engine/src/Core/Locator/AppWindowLocator.h"
#include "../../Engine/src/Window/AppWindow.h"
#include "../../Engine/src/Core/Time/Time.h"

SampleApplication::CameraController::CameraController(){}

void SampleApplication::CameraController::Awake()
{
    window = IHCEngine::Core::AppWindowLocator::GetAppWindow()->GetWindowHandle();
    camera = &(this->gameObject->GetScene()->GetCamera());
    // Editor Input
    pitch = 0;
    yaw = 0;
    camera->transform.Rotate(glm::vec3(-20, 0, 0));
    camera->transform.SetPosition(glm::vec3(0.0f, 10.8f, 18.5f));

    // Animation Viewer Input
    glm::vec3 cameraPosWithoutY = camera->transform.GetPosition();
    angleRespectToCenterPoint = 90;
    cameraPosWithoutY.y = 0;
    distanceToCenterPoint = length(centerPoint - cameraPosWithoutY);
    camera->LookAt(glm::vec3(0, 5, 0));
}

void SampleApplication::CameraController::Start()
{
}
void SampleApplication::CameraController::Update()
{
    HandleAnimationViewerInput();
    //HandleEditorInput();
}
void SampleApplication::CameraController::FixedUpdate(){}
void SampleApplication::CameraController::OnEnable(){}
void SampleApplication::CameraController::OnDisable(){}

void SampleApplication::CameraController::HandleAnimationViewerInput()
{
    float dt = IHCEngine::Core::Time::GetDeltaTime();
    auto cameraUp = camera->GetUp();
    auto cameraRight = camera->GetRight();
    auto cameraForward = camera->GetFoward();


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

void SampleApplication::CameraController::HandleEditorInput()
{
    // Handle movement:
    float dt = IHCEngine::Core::Time::GetDeltaTime();

    auto cameraUp = camera->GetUp();
    auto cameraRight = camera->GetRight();
    auto cameraForward = camera->GetFoward();

    // Translate
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_W))
    {
        auto p =  cameraForward * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_S))
    {
        auto p = -1.0f * cameraForward * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_A))
    {
        auto p = -1.0f * cameraRight * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_D))
    {
        auto p = cameraRight * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_LEFT_SHIFT))
    {
        auto p = cameraUp * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_LEFT_CONTROL))
    {
        auto p = -1.0f * cameraUp * movementSpeed * dt;
        camera->transform.Translate(p);
    }

    // Rotate

    if (IHCEngine::Core::Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT) && !isRotating)
    {
        glfwGetCursorPos(window, &lastX, &lastY);
        isRotating = true;
    }
    else if (IHCEngine::Core::Input::IsMouseButtonUp(GLFW_MOUSE_BUTTON_RIGHT) && isRotating)
    {
        isRotating = false;
    }
    if (isRotating)
    {
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float mouseDeltaX = rotationSpeed * (mouseX - lastX) * dt; //glm::radians((mouseX - lastX) * dt);
        float mouseDeltaY = rotationSpeed * (mouseY - lastY) * dt; // glm::radians((mouseY - lastY) * dt);

        yaw += mouseDeltaX;
        pitch += mouseDeltaY;
        camera->transform.SetRotation(glm::vec3(pitch, yaw, 0.0f));


        lastX = mouseX;
        lastY = mouseY;
    }

    // Zoom in & out
    auto scrollDelta = IHCEngine::Core::Input::GetScrollDelta();
    if (scrollDelta.y != 0.0)
    {
        float newFov = camera->GetFOV() - static_cast<float>(scrollDelta.y) * zoomSpeed ;
        //newFov = std::clamp(newFov, 1.0f, 179.0f);
        camera->SetFOV(newFov);
    }

}
