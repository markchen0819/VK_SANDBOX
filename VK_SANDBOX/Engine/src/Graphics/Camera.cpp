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
    viewMatrix = glm::inverse(transform.GetWorldMatrix());
    return viewMatrix;
}

glm::mat4 IHCEngine::Graphics::Camera::GetInverseViewMatrix()
{
    inverseViewMatrix = transform.GetWorldMatrix();
    return inverseViewMatrix;
}

void IHCEngine::Graphics::Camera::SetCameraType(CameraType type)
{
    this->type = type;
    GetProjectionMatrix(); // update immediately
}

