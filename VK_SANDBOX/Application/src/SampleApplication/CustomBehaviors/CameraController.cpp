#include <glm/gtx/string_cast.hpp> 
#include "CameraController.h"

#include "../../Engine/src/Graphics/Camera.h"
#include "../../Engine/src/Core/Scene/Scene.h"
#include "../../Engine/src/Core/Scene/GameObject.h"
#include "../../Engine/src/Core/Locator/AppWindowLocator.h"
#include "../../Engine/src/Core/Time/Time.h"

SampleApplication::CameraController::CameraController()
{


}

void SampleApplication::CameraController::Awake()
{
    window = IHCEngine::Core::AppWindowLocator::GetAppWindow()->GetWindowHandle();
    camera = &(this->gameObject->GetScene()->GetCamera());
    camera->transform.SetWorldPosition(glm::vec3(0.0f, 0.0f, 10.0f));
}

void SampleApplication::CameraController::Start()
{
}

void SampleApplication::CameraController::Update()
{
    HandleInput();
    //std::cout << glm::degrees(camera->transform.GetWorldRotation().x) << ",";
    //std::cout << glm::degrees(camera->transform.GetWorldRotation().y) << ",";
    //std::cout << glm::degrees(camera->transform.GetWorldRotation().z) << std::endl;
}

void SampleApplication::CameraController::FixedUpdate()
{
}

void SampleApplication::CameraController::OnEnable()
{
}

void SampleApplication::CameraController::OnDisable()
{
}

void SampleApplication::CameraController::HandleInput()
{
    // Handle movement:
    float dt = IHCEngine::Core::Time::GetInstance().GetDeltaTime();
    auto cameraUp = camera->transform.GetUp();
    auto cameraRight = camera->transform.GetRight();
    auto cameraForward = glm::cross(cameraUp,cameraRight);

    // Translate
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        auto p =  cameraForward * movementSpeed * dt;
        camera->transform.Translate(p, IHCEngine::Component::Space::Local);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        auto p = -1.0f * cameraForward * movementSpeed * dt;
        camera->transform.Translate(p, IHCEngine::Component::Space::Local);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        auto p = -1.0f * cameraRight * movementSpeed * dt;
        camera->transform.Translate(p, IHCEngine::Component::Space::Local);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        auto p = cameraRight * movementSpeed * dt;
        camera->transform.Translate(p, IHCEngine::Component::Space::Local);
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

        float mouseDeltaX = (mouseX - lastX) * dt; //glm::radians((mouseX - lastX) * dt);
        float mouseDeltaY = (mouseY - lastY) * dt; // glm::radians((mouseY - lastY) * dt);

        camera->transform.Rotate(glm::vec3(-mouseDeltaY, mouseDeltaX, 0.0f), IHCEngine::Component::Space::Local);
        // Using negative mouseDeltaY to make the rotation intuitive

        lastX = mouseX;
        lastY = mouseY;
    }

}
