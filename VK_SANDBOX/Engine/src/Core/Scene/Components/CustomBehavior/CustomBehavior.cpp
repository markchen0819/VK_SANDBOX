#include "../../../../pch.h"
#include "CustomBehavior.h"
#include "../../GameObject.h"

bool IHCEngine::Component::CustomBehavior::IsActive()
{
	return gameObject->IsActive() && this->isActive;
}
