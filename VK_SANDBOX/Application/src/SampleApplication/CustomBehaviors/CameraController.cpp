#include <glm/gtx/string_cast.hpp> 
#include "CameraController.h"

#include "../../Engine/src/Graphics/Camera.h"
#include "../../Engine/src/Core/Scene/Scene.h"
#include "../../Engine/src/Core/Scene/GameObject.h"
#include "../../Engine/src/Core/Locator/AppWindowLocator.h"
#include "../../Engine/src/Core/Time/Time.h"

SampleApplication::CameraController::CameraController(){}

void SampleApplication::CameraController::Awake()
{
    window = IHCEngine::Core::AppWindowLocator::GetAppWindow()->GetWindowHandle();
    camera = &(this->gameObject->GetScene()->GetCamera());
    camera->transform.Rotate(glm::vec3(20, 180, 0));
    camera->transform.SetWorldPosition(glm::vec3(0.0f, 10.0f, 20.0f));
    pitch = -20;
    yaw = 180;
}

void SampleApplication::CameraController::Start()
{
}
void SampleApplication::CameraController::Update()
{
    HandleInput();
}
void SampleApplication::CameraController::FixedUpdate(){}
void SampleApplication::CameraController::OnEnable(){}
void SampleApplication::CameraController::OnDisable(){}

void SampleApplication::CameraController::HandleInput()
{
    // Handle movement:
    float dt = IHCEngine::Core::Time::GetInstance().GetDeltaTime();

    auto cameraUp = camera->GetUp();
    auto cameraRight = camera->GetRight();
    auto cameraForward = camera->GetFoward();

    // Translate
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        auto p =  cameraForward * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        auto p = -1.0f * cameraForward * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        auto p = cameraRight * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        auto p = -1.0f * cameraRight * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        auto p = cameraUp * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        auto p = -1.0f * cameraUp * movementSpeed * dt;
        camera->transform.Translate(p);
    }

    // Rotate

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !isRotating)
    {
        glfwGetCursorPos(window, &lastX, &lastY);
        isRotating = true;
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && isRotating)
    {
        isRotating = false;
    }
    if (isRotating)
    {
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float mouseDeltaX = -rotationSpeed * (mouseX - lastX) * dt; //glm::radians((mouseX - lastX) * dt);
        float mouseDeltaY = rotationSpeed * (mouseY - lastY) * dt; // glm::radians((mouseY - lastY) * dt);

        // gimbal lock prevention
        yaw += mouseDeltaX;
        pitch += mouseDeltaY;
        // Convert pitch and yaw to a quaternion
        glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
        glm::quat qYaw = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));
        glm::quat orientation = qPitch * qYaw;
        camera->transform.SetRotationInQuaternion(orientation);

        // Euler (Gimbal lock)
        //camera->transform.Rotate(glm::vec3(-mouseDeltaY, mouseDeltaX, 0.0f));

        lastX = mouseX;
        lastY = mouseY;
    }

}
