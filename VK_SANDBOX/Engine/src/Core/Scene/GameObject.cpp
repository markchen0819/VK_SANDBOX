#include "../../pch.h"
#include "GameObject.h"

IHCEngine::Core::GameObject::GameObject(unsigned int id, std::string name, Scene* scene)
    :uid{ id }, name{ name }, scene{scene}
{
    transform.SetOwner(this);
    // transform component is added in default for all gameobjects
}

void IHCEngine::Core::GameObject::Destroy()
{
    shouldDestroy = true;
    isActive = false;
}

void IHCEngine::Core::GameObject::DestroyGameObject()
{

    // destroy child
    // destroy components

    texture = nullptr;
    model = nullptr;
}
