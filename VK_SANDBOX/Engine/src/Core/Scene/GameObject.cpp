#include "../../pch.h"
#include "GameObject.h"

unsigned int IHCEngine::Core::GameObject::currentId = 0;

IHCEngine::Core::GameObject::GameObject()
{
    currentId++;
    uid = currentId;
};