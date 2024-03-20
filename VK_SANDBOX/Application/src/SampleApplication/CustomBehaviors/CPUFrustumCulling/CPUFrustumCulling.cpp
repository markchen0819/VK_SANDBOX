#include "CPUFrustumCulling.h"
#include "../../../../../Engine/src/Core/Locator/SceneManagerLocator.h"
#include "../../../../../Engine/src/Input/Input.h"

SampleApplication::CPUFrustumCulling::CPUFrustumCulling()
{
    UpdateFrustum();
}


void SampleApplication::CPUFrustumCulling::Start()
{

}

void SampleApplication::CPUFrustumCulling::Update()
{
    UpdateFrustum();
    Cull();

    if (IHCEngine::Core::Input::IsKeyDown(GLFW_KEY_Z))
    {
        dropFrustum = !dropFrustum;
    }
}

void SampleApplication::CPUFrustumCulling::OnEnable()
{

}
void SampleApplication::CPUFrustumCulling::OnDisable()
{

}

void SampleApplication::CPUFrustumCulling::AddAABBWithGameObject(AABB_BoundingVolume aabb,
	IHCEngine::Core::GameObject* gobj)
{
    aabbs.push_back(std::move(aabb));
    gobjs.push_back(gobj);
}

void SampleApplication::CPUFrustumCulling::UpdateFrustum()
{
	auto camera = IHCEngine::Core::SceneManagerLocator::GetSceneManager()->GetActiveScene()->GetCamera();
    const glm::vec3 cameraPos = camera.transform.GetWorldPosition();

    if(cameraPrevPos != cameraPos && dropFrustum == false)
    {
        const float fov = camera.GetFOV();
        const float aspect = camera.GetAspectRatio();
        const float near = camera.GetNearClippingPlane();
        const float far =  camera.GetFarClippingPlane();

        glm::vec3 defaultForward(0, 0, -1);
        glm::vec3 defaultUp(0, 1, 0);
        const glm::vec3 frontDir = glm::normalize(camera.transform.GetRotationInQuaternion() * defaultForward);
        const glm::vec3 rightDir = glm::normalize(glm::cross(frontDir, defaultUp));
        const glm::vec3 upDir = glm::normalize(glm::cross(rightDir, frontDir));

        const float halfFarPlaneHeight = far * tanf(fov * 0.5f);
        const float halfFarPlaneWidth = halfFarPlaneHeight * aspect;
        const glm::vec3 vectorToFarPlane = far * frontDir;
        frustum.nearFace = { cameraPos + near * frontDir , frontDir };
        frustum.farFace = { cameraPos + far * frontDir, -frontDir };
        frustum.rightFace = { cameraPos, glm::cross(vectorToFarPlane - rightDir * halfFarPlaneWidth, upDir) };
        frustum.leftFace = { cameraPos, glm::cross(upDir,vectorToFarPlane + rightDir * halfFarPlaneWidth) };
        frustum.topFace = { cameraPos,glm::cross(rightDir, vectorToFarPlane - upDir * halfFarPlaneHeight) };
        frustum.bottomFace = { cameraPos,glm::cross(vectorToFarPlane + upDir * halfFarPlaneHeight, rightDir) };

    }
}

void SampleApplication::CPUFrustumCulling::Cull()
{
    for(int i=0; i < aabbs.size(); ++i)
    {
        if (aabbs[i].IsWithinFrustum(frustum))
        {
            gobjs[i]->SetActive(true);
        }
        else
        {
            gobjs[i]->SetActive(false);
        }
	    
    }
}
