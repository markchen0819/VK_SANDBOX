#include <glm/gtx/string_cast.hpp> 
#include "CameraController.h"

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
    float dt = IHCEngine::Core::Time::GetInstance().GetDeltaTime();
    auto cameraUp = camera->GetUp();
    auto cameraRight = camera->GetRight();
    auto cameraForward = camera->GetFoward();


    if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS) // rotate right around target
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
    if (glfwGetKey(window, GLFW_KEY_KP_DECIMAL) == GLFW_PRESS)// rotate left around target
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
    if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS) // reset
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
    if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) // left
    {
        auto p = -cameraRight * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) // right
    {
        auto p = cameraRight * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) // up
    {
        auto p = cameraUp * movementSpeed * dt;
        camera->transform.Translate(p);
        auto cameraX = camera->transform.GetPosition().x;
        camera->LookAt(glm::vec3(cameraX, 5, 0));
    }
    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) // down
    {
        auto p = -cameraUp * movementSpeed * dt;
        camera->transform.Translate(p);
        auto cameraX = camera->transform.GetPosition().x;
        camera->LookAt(glm::vec3(cameraX, 5, 0));
    }
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) // zoom in
    {
        float newFov = camera->GetFOV() - 1 * zoomSpeed;
        camera->SetFOV(newFov);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) // zoom in
    {
        float newFov = camera->GetFOV() + 1 * zoomSpeed;
        camera->SetFOV(newFov);
    }
}

void SampleApplication::CameraController::HandleEditorInput()
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
        auto p = -1.0f * cameraRight * movementSpeed * dt;
        camera->transform.Translate(p);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        auto p = cameraRight * movementSpeed * dt;
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

        float mouseDeltaX = rotationSpeed * (mouseX - lastX) * dt; //glm::radians((mouseX - lastX) * dt);
        float mouseDeltaY = rotationSpeed * (mouseY - lastY) * dt; // glm::radians((mouseY - lastY) * dt);

        yaw += mouseDeltaX;
        pitch += mouseDeltaY;
        const float MAX_PITCH = 89.0f;
        pitch = glm::clamp(pitch, -MAX_PITCH, MAX_PITCH);
        // Convert pitch and yaw to a quaternion
        glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
        glm::quat qYaw = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));
        glm::quat orientation = qPitch * qYaw;
        camera->transform.SetRotationInQuaternion(orientation);

        lastX = mouseX;
        lastY = mouseY;
    }

    auto window = IHCEngine::Core::AppWindowLocator::GetAppWindow();
    // Zoom in & out
    auto scrollDelta = window->GetScrollOffset();
    if (scrollDelta.y != 0.0)
    {
        float newFov = camera->GetFOV() - static_cast<float>(scrollDelta.y) * zoomSpeed ;
        //newFov = std::clamp(newFov, 1.0f, 179.0f);
        camera->SetFOV(newFov);
    }

}
