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
    camera->transform.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    camera->transform.SetPosition(glm::vec3(0.0f, 10.0f, 30.0f));
}

void SampleApplication::CameraController::Start()
{
}
void SampleApplication::CameraController::Update()
{
    HandleEditorInput();
}
void SampleApplication::CameraController::FixedUpdate(){}
void SampleApplication::CameraController::OnEnable(){}
void SampleApplication::CameraController::OnDisable(){}

void SampleApplication::CameraController::HandleEditorInput()
{
    // Handle movement:
    float dt = IHCEngine::Core::Time::GetDeltaTime();

    auto up = camera->transform.GetUp();
    auto right = camera->transform.GetRight();
    auto forward = camera->transform.GetForward();

    // Translate
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_W))
    {
        auto p = -1.0f * forward * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_S))
    {
        auto p = forward * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_A))
    {
        auto p = -1.0f * right * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_D))
    {
        auto p = right * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_LEFT_SHIFT))
    {
        auto p = up * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_LEFT_CONTROL))
    {
        auto p = -1.0f * up * movementSpeed * dt;
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

        float mouseDeltaX = rotationSpeed * static_cast<float>(mouseX - lastX) * dt; //glm::radians((mouseX - lastX) * dt);
        float mouseDeltaY = rotationSpeed * static_cast<float>(mouseY - lastY) * dt; // glm::radians((mouseY - lastY) * dt);

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
