#include "../../../pch.h"
#include "TextureDescriptorWrap.h"

#include "../IHCSwapChain.h"
#include "../IHCBuffer.h"
#include "../VKHelpers.h"

namespace IHCEngine::Graphics
{
	TextureDescriptorWrap::TextureDescriptorWrap(IHCDevice& device)
		: IHCDescriptorWrap(device)
	{
	}

	TextureDescriptorWrap::~TextureDescriptorWrap()
	{
	}

	void TextureDescriptorWrap::Setup()
	{
		SetMaxSets(TEXTURE_COUNT_LIMIT* IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			TEXTURE_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		InitializePool();

		AddLayoutBinding
		(
			0, 
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 
			VK_SHADER_STAGE_FRAGMENT_BIT
		);
		CreateLayout();
	}

	void TextureDescriptorWrap::CreateResources()
	{
		// Texture resources are created in IHCTexture
	}

	void TextureDescriptorWrap::AllocAndBindDescriptorSets()
	{
		// Do bindings at  Texture resources creation

		// example:
		//auto targetSet = AllocateDescriptorSet();
		//VkDescriptorImageInfo targetImageInfo{};
		//targetImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		//targetImageInfo.imageView = texture->GetTextureImageView();
		//targetImageInfo.sampler = texture->GetTextureSampler();
		//int binding = 0;
		//BindImage(binding, &targetImageInfo);
		//Overwrite(targetSet);
	}
}
