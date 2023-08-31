#include "../pch.h"
#include "Camera.h"

//void IHCEngine::Graphics::Camera::SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far) 
//{
//    projectionMatrix = 
//    {
//        {2.f / (right - left), 0.f, 0.f, 0.f},
//        {0.f, 2.f / (bottom - top), 0.f, 0.f},
//        {0.f, 0.f, 1.f / (far - near), 0.f},
//        {-(right + left) / (right - left), -(bottom + top) / (bottom - top), -near / (far - near), 1.f}
//    };
//    // scales the coordinates of the 3D objects 
//    // fit within the viewport dimensions
//    // translates them to be centered in the view.
//
//}
//
//void IHCEngine::Graphics::Camera::SetPerspectiveProjection(float fovy, float aspect, float near, float far) 
//{
//    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
//    const float tanHalfFovy = tan(fovy / 2.f);
//    projectionMatrix = 
//    {
//        {1.f / (aspect * tanHalfFovy), 0.f, 0.f, 0.f},
//        {0.f, 1.f / (tanHalfFovy), 0.f, 0.f},
//        {0.f, 0.f, far / (far - near), 1.f},
//        {0.f, 0.f, -(far * near) / (far - near), 0.f}
//    };
//
//    // perspective divide
//    // scaling objects based on depth for perspective effect. 
//    // depth linearly scaled to fit within the near and far clipping planes.
//
//}
//
//void IHCEngine::Graphics::Camera::SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
//{
//    const glm::vec3 w{glm::normalize(direction)};
//    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
//    const glm::vec3 v{glm::cross(w, u)};
//
//    viewMatrix = 
//    {
//        {u.x, v.x, w.x, 0.f},
//        {u.y, v.y, w.y, 0.f},
//        {u.z, v.z, w.z, 0.f},
//        {-glm::dot(u, position), -glm::dot(v, position), -glm::dot(w, position), 1.f}
//    };    
//
//    inverseViewMatrix = 
//    {
//        {u.x, u.y, u.z, 0.f},
//        {v.x, v.y, v.z, 0.f},
//        {w.x, w.y, w.z, 0.f},
//        {position.x, position.y, position.z, 1.f}
//    };
//
//    // define the right, up, and forward directions of the camera
//    // translates objects based on the camera's position in world space
//}
//
//void IHCEngine::Graphics::Camera::SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up)
//{
//   SetViewDirection(position, target - position, up);
//}
//
//void IHCEngine::Graphics::Camera::SetViewYXZ(glm::vec3 position, glm::vec3 rotation)
//{
//    const float c3 = glm::cos(rotation.z);
//    const float s3 = glm::sin(rotation.z);
//    const float c2 = glm::cos(rotation.x);
//    const float s2 = glm::sin(rotation.x);
//    const float c1 = glm::cos(rotation.y);
//    const float s1 = glm::sin(rotation.y);
//    const glm::vec3 u{(c1* c3 + s1 * s2 * s3), (c2* s3), (c1* s2* s3 - c3 * s1)};
//    const glm::vec3 v{(c3* s1* s2 - c1 * s3), (c2* c3), (c1* c3* s2 + s1 * s3)};
//    const glm::vec3 w{(c2* s1), (-s2), (c1* c2)};
//
//    viewMatrix = 
//    {
//        {u.x, v.x, w.x, 0.f},
//        {u.y, v.y, w.y, 0.f},
//        {u.z, v.z, w.z, 0.f},
//        {-glm::dot(u, position), -glm::dot(v, position), -glm::dot(w, position), 1.f}
//    };
//    inverseViewMatrix = 
//    {
//        {u.x, u.y, u.z, 0.f},
//        {v.x, v.y, v.z, 0.f},
//        {w.x, w.y, w.z, 0.f},
//        {position.x, position.y, position.z, 1.f}
//    };
//}

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

void IHCEngine::Graphics::Camera::Update()
{

}
