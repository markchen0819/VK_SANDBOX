#include "../../pch.h"
#include "GameObject.h"

IHCEngine::Core::GameObject::GameObject(unsigned int id, std::string name)
    :uid{ id }, name{ name }
{}

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
