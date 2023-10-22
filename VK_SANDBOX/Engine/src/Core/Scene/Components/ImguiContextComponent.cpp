#include "../../../pch.h"
#include "ImguiContextComponent.h"

#include "../../Locator/ImGuiManagerLocator.h"

IHCEngine::Component::ImguiContextComponent::ImguiContextComponent()
	: Component(ComponentType ::ImguiContext)
{
	Core::ImGuiManagerLocator::GetImGuiManager()->RegisterImguiContextComponent(this);
}


void IHCEngine::Component::ImguiContextComponent::Remove()
{
	Core::ImGuiManagerLocator::GetImGuiManager()->RemoveImguiContextComponent(this);

}