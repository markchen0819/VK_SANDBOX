#include "../../../pch.h"
#include "MeshComponent.h"

#include "../../Locator/GraphicsManagerLocator.h"
#include "../../../Graphics/RenderSystems/RenderSystem.h"
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

void IHCEngine::Component::MeshComponent::InstanceDraw(VkCommandBuffer commandBuffer, int instanceCount)
{
	mesh->InstanceDraw(commandBuffer, instanceCount);
}

void IHCEngine::Component::MeshComponent::Attach()
{
	Core::GraphicsManagerLocator::GetGraphicsManager()->GetRenderSystem().AddGameObjectToRender(this->gameObject, Graphics::PipelineType::DEFAULT);
}

void IHCEngine::Component::MeshComponent::Remove()
{
	Core::GraphicsManagerLocator::GetGraphicsManager()->GetRenderSystem().RemoveGameObjectToRender(this->gameObject, Graphics::PipelineType::DEFAULT);
}
