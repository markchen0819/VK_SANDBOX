#include "../pch.h"
#include "Camera.h"

IHCEngine::Graphics::Camera::Camera(CameraType type, float fov, float aspect, float near, float far, float orthoWidth, float orthoHeight)
	: type(type), 
	  fov(fov), 
	  aspectRatio(aspect),
	  nearPlane(near), 
	  farPlane(far),
     orthoWidth(orthoWidth),
     orthoHeight(orthoHeight)
{ 

}

glm::mat4 IHCEngine::Graphics::Camera::GetProjectionMatrix()
{
    if (!isProjectionDirty) return projectionMatrix; // cache to save performance

    if (type == CameraType::PERSPECTIVE) 
    {
        projectionMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
    }
    else
    {
        float left = -orthoWidth / 2.0f;
        float right = orthoWidth / 2.0f;
        float bottom = -orthoHeight / 2.0f;
        float top = orthoHeight / 2.0f;
        projectionMatrix = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
    }
    return projectionMatrix;
}

glm::mat4 IHCEngine::Graphics::Camera::GetViewMatrix()
{
    // inverse transformation move the world 
    // such that the camera's position is at the origin.
    // inverse rotation will orient the world 
    // such that the camera's forward direction aligns with
    // the desired view direction (e.g., the negative z-axis in a right-handed coordinate system).

    viewMatrix = glm::inverse(transform.GetModelMatrix());
    return viewMatrix;
}

glm::mat4 IHCEngine::Graphics::Camera::GetInverseViewMatrix()
{
    inverseViewMatrix = glm::inverse(GetViewMatrix());;
    return inverseViewMatrix;
}

void IHCEngine::Graphics::Camera::LookAt(glm::vec3 target)
{
    viewMatrix = glm::lookAt(transform.GetPosition(),
        target, glm::vec3(0,1,0));
    // Extracting the camera's rotation:
    glm::mat3 rotationMat3 = glm::inverse(glm::mat3(viewMatrix));
    glm::quat cameraRotation = glm::quat_cast(rotationMat3);
    transform.SetRotationInQuaternion(cameraRotation);
}

void IHCEngine::Graphics::Camera::SetCameraType(CameraType type)
{
    this->type = type;
    GetProjectionMatrix(); // update immediately
}

