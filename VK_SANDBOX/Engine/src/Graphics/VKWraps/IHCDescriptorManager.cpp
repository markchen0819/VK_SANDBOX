#include "../../pch.h"
#include "IHCDescriptorManager.h"

#include "IHCSwapChain.h"
#include "IHCDevice.h"
#include "IHCBuffer.h"
#include "VKHelpers.h"

#include "IHCTexture.h"
#include "../Animation/Animator.h"
#include "../Animation/InverseKinematicsSolver.h"

namespace IHCEngine::Graphics
{
	IHCDescriptorManager::IHCDescriptorManager(IHCDevice& ihcDevice)
		: ihcDevice(ihcDevice)
	{
		globalDescriptorWrap = std::make_unique<GlobalDescriptorWrap>(ihcDevice);
		textureDescriptorWrap = std::make_unique<TextureDescriptorWrap>(ihcDevice);
		skeletalDescriptorWrap = std::make_unique<SkeletalDescriptorWrap>(ihcDevice);
		globalDescriptorWrap->Setup();
		textureDescriptorWrap->Setup();
		skeletalDescriptorWrap->Setup();
	}

	std::vector<VkDescriptorSetLayout> IHCDescriptorManager::GetDefaultDescriptorSetLayoutsForBasicRenderSystem()
	{
		// In GLSL, our case
		// globalDescriptorSetLayout set 0, Binding0, UNIFORM_BUFFER
		// textureDescriptorSetLayout set 1, Binding0, COMBINED_IMAGE_SAMPLER
		std::vector<VkDescriptorSetLayout> layouts {
			globalDescriptorWrap->GetDescriptorSetLayout(),
			textureDescriptorWrap->GetDescriptorSetLayout(),
		};
		return layouts;
	}

	void IHCDescriptorManager::AllocateTextureDescriptorSetForTexture(IHCTexture* texture)
	{
		auto assetName = texture->GetName();

		// check if already allocate descriptorsets for the texture;
		if(!texture->GetDescriptorSets().empty())
		{
			assert("Loading Duplicated Texture into memory, abort");
		}
	
		// allocate MAX_FRAMES_IN_FLIGHT descriptorsets for 1 texture
		std::vector<VkDescriptorSet> descriptorSetsForTexture;
		for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto targetDesciptorSet = textureDescriptorWrap->AllocateDescriptorSet();

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = texture->GetTextureImageView();
			imageInfo.sampler = texture->GetTextureSampler();

			int binding = 0;
			if(texture->GetTextureType() == TextureType::DIFFUSE ||
				texture->GetTextureType() == TextureType::NOTSPECIFIED)
			{
				binding = 0;
			}
			// else if() add other types....

			textureDescriptorWrap->BindImage(binding, &imageInfo);
			textureDescriptorWrap->Overwrite(targetDesciptorSet);

			descriptorSetsForTexture.push_back(targetDesciptorSet);
		}
		texture->SetDescriptorSets(descriptorSetsForTexture);
	}
	void IHCDescriptorManager::DeallocateTextureDescriptorSetForTexture(IHCTexture* texture)
	{
		// All submitted commands that refer to sampler must have completed execution
		vkDeviceWaitIdle(ihcDevice.GetDevice());

		const std::vector<VkDescriptorSet>& textureDescriptorSets = texture->GetDescriptorSets();

		if (textureDescriptorSets.empty())
		{
			assert("Clearing a texture descriptor sets were not allocated, shouldnt be here ");
		}
		// Push back each descriptor set to the available pool 
		for (VkDescriptorSet descriptor : textureDescriptorSets)
		{
			textureDescriptorWrap->FreeDescriptorSet(descriptor);
		}
		texture->SetDescriptorSets({});
	}
	void IHCDescriptorManager::AllocateSkeletalDescriptorSetForAnimator(Animator* animator)
	{
		// check if already allocate descriptors for animator
		if (!animator->GetDescriptorSets().empty())
		{
			assert("Duplicated animator");
		}
		// allocate MAX_FRAMES_IN_FLIGHT descriptorsets for 1 animator
		std::vector<VkDescriptorSet> descriptorSetsForAnimator;
		std::vector<IHCBuffer*> skeletalUBOsForAnimator;

		for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto targetDescriptorSet = skeletalDescriptorWrap->AllocateDescriptorSet();
			auto targetBuffer = skeletalDescriptorWrap->GetAvailableSkeletalUBO();
			auto bufferInfo = targetBuffer->GetDescriptorInfo();
			skeletalDescriptorWrap->BindBuffer(0, &bufferInfo);
			skeletalDescriptorWrap->Overwrite(targetDescriptorSet);
			descriptorSetsForAnimator.push_back(targetDescriptorSet);
			skeletalUBOsForAnimator.push_back(targetBuffer);
		}
		animator->SetDescriptorSets(descriptorSetsForAnimator);
		animator->SetBuffers(skeletalUBOsForAnimator);
	}
	void IHCDescriptorManager::DeallocateSkeletalDescriptorSetForAnimator(Animator* animator)
	{
		// All submitted commands that refer to sampler must have completed execution
		vkDeviceWaitIdle(ihcDevice.GetDevice());

		const std::vector<VkDescriptorSet>& animatorDescriptorSets = animator->GetDescriptorSets();
		if (animatorDescriptorSets.empty())
		{
			assert("Clearing a skeletal descriptor sets were not allocated, shouldnt be here ");
		}
		// Push back each descriptor set to the available pool 
		for (VkDescriptorSet descriptor : animatorDescriptorSets)
		{
			skeletalDescriptorWrap->FreeDescriptorSet(descriptor);
		}
		animator->SetDescriptorSets({});

		const std::vector<IHCBuffer*>& animatorBuffers = animator->GetBuffers();
		if (animatorBuffers.empty())
		{
			assert("Clearing a skeletal buffers were not allocated, shouldnt be here ");
		}
		// Push back each buffer  to the available pool 
		for (IHCBuffer* buffer : animatorBuffers)
		{
			skeletalDescriptorWrap->ReleaseSkeletalUBO(buffer);
		}
		animator->SetBuffers({});
	}

	void IHCDescriptorManager::AllocateSkeletalDescriptorSetForIK(InverseKinematicsSolver* ikSolver)
	{
		// check if already allocate descriptors for animator
		if (!ikSolver->GetDescriptorSets().empty())
		{
			assert("Duplicated ikSolver");
		}
		// allocate MAX_FRAMES_IN_FLIGHT descriptorsets for 1 animator
		std::vector<VkDescriptorSet> descriptorSetsForIK;
		std::vector<IHCBuffer*> skeletalUBOsForIK;

		for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto targetDescriptorSet = skeletalDescriptorWrap->AllocateDescriptorSet();
			auto targetBuffer = skeletalDescriptorWrap->GetAvailableSkeletalUBO();
			auto bufferInfo = targetBuffer->GetDescriptorInfo();
			skeletalDescriptorWrap->BindBuffer(0, &bufferInfo);
			skeletalDescriptorWrap->Overwrite(targetDescriptorSet);
			descriptorSetsForIK.push_back(targetDescriptorSet);
			skeletalUBOsForIK.push_back(targetBuffer);
		}
		ikSolver->SetDescriptorSets(descriptorSetsForIK);
		ikSolver->SetBuffers(skeletalUBOsForIK);
	}
	void IHCDescriptorManager::DeallocateSkeletalDescriptorSetForIK(InverseKinematicsSolver* ikSolver)
	{
		// All submitted commands that refer to sampler must have completed execution
		vkDeviceWaitIdle(ihcDevice.GetDevice());

		const std::vector<VkDescriptorSet>& IKDescriptorSets = ikSolver->GetDescriptorSets();
		if (IKDescriptorSets.empty())
		{
			assert("Clearing a skeletal descriptor sets were not allocated, shouldnt be here ");
		}
		// Push back each descriptor set to the available pool 
		for (VkDescriptorSet descriptor : IKDescriptorSets)
		{
			skeletalDescriptorWrap->FreeDescriptorSet(descriptor);
		}
		ikSolver->SetDescriptorSets({});

		const std::vector<IHCBuffer*>& animatorBuffers = ikSolver->GetBuffers();
		if (animatorBuffers.empty())
		{
			assert("Clearing a skeletal buffers were not allocated, shouldnt be here ");
		}
		// Push back each buffer  to the available pool 
		for (IHCBuffer* buffer : animatorBuffers)
		{
			skeletalDescriptorWrap->ReleaseSkeletalUBO(buffer);
		}
		ikSolver->SetBuffers({});

	}
}
