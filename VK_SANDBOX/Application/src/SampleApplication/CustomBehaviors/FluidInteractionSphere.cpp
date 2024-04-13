#include "FluidInteractionSphere.h"

#include "../../../../Engine/src/Core/Scene/Components/ComputeFluidComponent.h"
#include "../../../../Engine/src/Input/Input.h"
#include "../../Engine/src/Core/Scene/Scene.h"
#include "../../Engine/src/Core/Scene/GameObject.h"
#include "../../Engine/src/Core/Time/Time.h"

void SampleApplication::FluidInteractionSphere::Awake()
{
    camera = &(this->gameObject->GetScene()->GetCamera());
}

void SampleApplication::FluidInteractionSphere::Update()
{
    float dt = IHCEngine::Core::Time::GetDeltaTime();

    auto right = camera->transform.GetRight();
    auto forward = camera->transform.GetForward();

    right = glm::vec3(right.x, 0.0f, right.z);
    forward = glm::vec3(forward.x, 0.0f, forward.z);
    auto down = glm::normalize(glm::cross(right, forward));

    auto pos = this->gameObject->transform.GetPosition();

    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_I))
    {
        pos += -forward * movementSpeed * dt;
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_K))
    {
        pos += forward * movementSpeed * dt;
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_J))
    {
        pos += -right * movementSpeed * dt;
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_L))
    {
        pos += right * movementSpeed * dt;
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_O))
    {
        pos += -down * movementSpeed * dt;
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_U))
    {
        pos += down * movementSpeed * dt;
    }

    this->gameObject->transform.SetPosition(pos);
    computeFluid->SetInteractionSpherePos(glm::vec4(pos, 1.0f));
}

