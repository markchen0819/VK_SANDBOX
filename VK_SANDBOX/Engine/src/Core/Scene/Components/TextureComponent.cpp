#include "../../../pch.h"
#include "TextureComponent.h"

#include "../../../Graphics/VKWraps/IHCTexture.h"

IHCEngine::Component::TextureComponent::TextureComponent()
	:Component(ComponentType::Texture)
{
}

void IHCEngine::Component::TextureComponent::SetTexture(Graphics::IHCTexture* texture)
{
	this->texture = texture;
}

std::vector<VkDescriptorSet>& IHCEngine::Component::TextureComponent::GetDescriptorSets()
{
	return texture->GetDescriptorSets();
}


void IHCEngine::Component::TextureComponent::Attach()
{
}

void IHCEngine::Component::TextureComponent::Remove()
{

}
