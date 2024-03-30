#include "GrassInteractionSphere.h"

#include "../../../../Engine/src/Core/Scene/Components/ComputeGrassComponent.h"
#include "../../../../Engine/src/Input/Input.h"
#include "../../Engine/src/Core/Scene/Scene.h"
#include "../../Engine/src/Core/Scene/GameObject.h"
#include "../../Engine/src/Core/Time/Time.h"


void SampleApplication::GrassInteractionSphere::Awake()
{
    camera = &(this->gameObject->GetScene()->GetCamera());


}

void SampleApplication::GrassInteractionSphere::Update()
{
    float dt = IHCEngine::Core::Time::GetDeltaTime();
    glm::vec3 acceleration = force / mass;
    velocity += acceleration * dt;

    float dragFactor = 1.0f - (0.6f * dt);
    velocity *= dragFactor;

    this->gameObject->transform.Translate(velocity * dt * movementScale);
    force = glm::vec3(0.0f);



    auto newPos = glm::vec4(this->gameObject->transform.GetWorldPosition(), 1.0f);
    newPos.y = 4.85f;
	for (auto c : computeGrassComponents)
    {
        c->SetSpherePosition(newPos);
    }

}

void SampleApplication::GrassInteractionSphere::FixedUpdate()
{
    auto right = camera->transform.GetRight();
    auto forward = camera->transform.GetForward();
    right = glm::vec3(right.x, 0.0f, right.z);
    forward = glm::vec3(forward.x, 0.0f, forward.z);


    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_UP))
    {
        force += -forward;
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_DOWN))
    {
        force += forward;
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_LEFT))
    {
        force += -right;
    }
    if (IHCEngine::Core::Input::IsKeyHeld(GLFW_KEY_RIGHT))
    {
        force += right;
    }
}

void SampleApplication::GrassInteractionSphere::AddComputeGrassComponent(IHCEngine::Component::ComputeGrassComponent* c)
{
    computeGrassComponents.push_back(c);
}
