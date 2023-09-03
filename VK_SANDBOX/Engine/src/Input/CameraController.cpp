#include "../pch.h"
#include "CameraController.h"
#include "../Graphics/Camera.h"
#include <glm/gtx/string_cast.hpp> 
IHCEngine::Input::CameraController::CameraController(IHCEngine::Graphics::Camera& camera)
    : camera(camera)
{
    glm::vec3 eyePosition = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    //glm::vec3 upVector = camera.transform.GetUp();//glm::vec3(0.0f, 0.0f, 1.0f);
    camera.transform.SetWorldPosition(eyePosition);
   // glm::vec3 direction = glm::normalize(targetPosition - eyePosition);
    //glm::quat rotation = glm::quatLookAt(direction, upVector);  // Generate the quaternion rotation from the look direction and the up vector
   // camera.transform.SetWorldRotation(rotation);  // Set the camera's rotation

}

void IHCEngine::Input::CameraController::HandleInput(GLFWwindow* window)
{
    // Define world axes
    glm::vec3 worldX = glm::vec3(1.0f, 0.0f, 0.0f); // right direction in world
    glm::vec3 worldY = glm::vec3(0.0f, 1.0f, 0.0f); // up direction in world
    glm::vec3 worldZ = glm::vec3(0.0f, 0.0f, 1.0f); // forward direction in world

    // Handle movement:
    
    auto cameraUp = camera.transform.GetUp();
    auto cameraRight = camera.transform.GetRight();
    auto cameraForward = glm::cross(cameraUp,cameraRight);
        //camera.transform.GetForward() * glm::vec3(0, 0, -1);
    //std::cout << "Z" << std::endl;
    //std::cout << glm::to_string(cameraForward) << std::endl;
    //std::cout << "Y" << std::endl;
    //std::cout << glm::to_string(cameraUp) << std::endl;
    //std::cout << "X" << std::endl;
    //std::cout << glm::to_string(cameraRight) << std::endl;
    // Movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        auto p = camera.transform.GetWorldPosition();
        p += cameraForward * movementSpeed;
        camera.transform.SetWorldPosition(p);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        auto p = camera.transform.GetWorldPosition();
        p -= cameraForward * movementSpeed;
        camera.transform.SetWorldPosition(p);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        auto p = camera.transform.GetWorldPosition();
        p -= cameraRight * movementSpeed;
        camera.transform.SetWorldPosition(p);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        auto p = camera.transform.GetWorldPosition();
        p += cameraRight * movementSpeed;
        camera.transform.SetWorldPosition(p);
    }


    // Get the current world rotation as a quaternion.
    glm::quat currentRotation = camera.transform.GetWorldRotation();

    // Handle rotations:
    float rotationAmount = glm::radians(rotationSpeed * -1);  // Convert degrees to radians.

    glm::quat pitchQuat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // No rotation by default.
    glm::quat yawQuat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);    // No rotation by default.
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 worldRight = glm::vec3(1.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        pitchQuat = glm::angleAxis(-rotationAmount, worldRight);  // Rotate around right axis.
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        pitchQuat = glm::angleAxis(rotationAmount, worldRight);   // Rotate around right axis.
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        yawQuat = glm::angleAxis(-rotationAmount, worldUp);       // Rotate around up axis.
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        yawQuat = glm::angleAxis(rotationAmount, worldUp);        // Rotate around up axis.
    }

    // Combine the rotations.
    glm::quat combinedRotation = currentRotation * yawQuat * pitchQuat;

    // Set the combined rotation.
    camera.transform.SetWorldRotation(combinedRotation);


}
