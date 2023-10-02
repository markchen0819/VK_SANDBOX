#include "../../../pch.h"
#include "MeshComponent.h"

#include "../../../Graphics/VKWraps/IHCMesh.h"

IHCEngine::Component::MeshComponent::MeshComponent()
	:Component(ComponentType::Mesh)
{
}

void IHCEngine::Component::MeshComponent::SetMesh(Graphics::IHCMesh* mesh)
{
	this->mesh = mesh;
}

void IHCEngine::Component::MeshComponent::Bind(VkCommandBuffer commandBuffer)
{
	mesh->Bind(commandBuffer);
}

void IHCEngine::Component::MeshComponent::Draw(VkCommandBuffer commandBuffer)
{
	mesh->Draw(commandBuffer);
}
void IHCEngine::Component::MeshComponent::Remove()
{
	
}