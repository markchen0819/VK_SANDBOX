#include "../../pch.h"
#include "IHCDescriptorManager.h"

#include "IHCSwapChain.h"
#include "IHCBuffer.h"
#include "IHCDescriptors.h"
#include "IHCDevice.h"
#include "IHCTexture.h"
#include "VKHelpers.h"
#include "../Animation/Animator.h"

namespace IHCEngine::Graphics
{
	IHCDescriptorManager::IHCDescriptorManager(IHCDevice& ihcDevice)
		: ihcDevice(ihcDevice)
	{
		createDescriptorSetLayouts(); // Set up shader interface for PipelineLayout
		initPool();
		createGlobalUniformBuffers();
		createCustomUniformBuffers();
		allocateGlobalDescriptorSets();
	}

	std::vector<VkDescriptorSetLayout> IHCDescriptorManager::GetDefaultDescriptorSetLayoutsForBasicRenderSystem()
	{
		// In GLSL, our case
		// globalDescriptorSetLayout set 0, Binding0, UNIFORM_BUFFER
		// textureDescriptorSetLayout set 1, Binding0, COMBINED_IMAGE_SAMPLER
		std::vector<VkDescriptorSetLayout> layouts {
			globalDescriptorSetLayout->GetDescriptorSetLayout(),
			textureDescriptorSetLayout->GetDescriptorSetLayout(),
		};
		return layouts;
	}

	void IHCDescriptorManager::createDescriptorSetLayouts()
	{
		globalDescriptorSetLayout =
			IHCDescriptorSetLayout::Builder(ihcDevice)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)  // Assuming this is for vertex shaders
			.Build();

		textureDescriptorSetLayout =
			IHCDescriptorSetLayout::Builder(ihcDevice)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)  // Assuming this is for fragment shaders
			.Build();

		skeletalDescriptorSetLayout =
			IHCDescriptorSetLayout::Builder(ihcDevice)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.Build();
	}

	void IHCDescriptorManager::initPool()
	{

		globalDescriptorPool =
			IHCDescriptorPool::Builder(ihcDevice)
			.SetMaxSets(IHCSwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, IHCSwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();

		localDescriptorPool =
			IHCDescriptorPool::Builder(ihcDevice)
			.SetMaxSets(
				(TEXTURE_COUNT_LIMIT + SKELETAL_COUNT_LIMIT) 
				* IHCSwapChain::MAX_FRAMES_IN_FLIGHT
			)
			.AddPoolSize(
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				TEXTURE_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT
			) // sampler
			.AddPoolSize(
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				SKELETAL_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT
			) // sampler
			.Build();

		globalDescriptorSets.resize(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		textureDescriptorSets.resize(TEXTURE_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		skeletalDescriptorSets.resize(SKELETAL_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);

		// Pre-populate the availableDescriptorSets stack:
		for (auto& descSet : textureDescriptorSets)
		{
			availableTextureDescriptorSets.push(descSet);
		}
		for (auto& descSet : skeletalDescriptorSets)
		{
			availableSkeletalDescriptorSets.push(descSet);
		}
	}

	void IHCDescriptorManager::createGlobalUniformBuffers()
	{
		// Allocate memory to store global shader data
		// (camera matrices, global light info)
		// Data that's consistent across a frame
		globalUBOs.resize(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalUBOs.size(); i++)
		{
			globalUBOs[i] = std::make_unique<IHCBuffer>
				(
					ihcDevice,
					sizeof(GlobalUniformBufferObject),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					//| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
					// we can uncomment this as we're not using minOffsetAlignment for now
				);
			globalUBOs[i]->Map(); // persistent mapping
		}
	}

	void IHCDescriptorManager::createCustomUniformBuffers()
	{
		skeletalUBOs.resize(SKELETAL_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < skeletalUBOs.size(); i++)
		{
			skeletalUBOs[i] = std::make_unique<IHCBuffer>
				(
					ihcDevice,
					sizeof(SkeletalUniformBufferObject),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					//| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
					// we can uncomment this as we're not using minOffsetAlignment for now
				);
			skeletalUBOs[i]->Map(); // persistent mapping
		}
		for (const auto& uboUniquePtr : skeletalUBOs)
		{
			availableSkeletalUBOs.push(uboUniquePtr.get());
		}
	}

	void IHCDescriptorManager::allocateGlobalDescriptorSets()
	{
		//  binding 0, UNIFORM_BUFFER
		for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto bufferInfo = globalUBOs[i]->GetDescriptorInfo();
			IHCDescriptorWriter(*globalDescriptorSetLayout, *globalDescriptorPool)
				.WriteBuffer(0, &bufferInfo) 
				.Build(globalDescriptorSets[i]);
		}
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
			if (availableTextureDescriptorSets.empty())
			{
				assert("No available descriptor sets for allocation. Check if exceed poolsize");
			}

			VkDescriptorSet descriptor = availableTextureDescriptorSets.top();
			availableTextureDescriptorSets.pop();

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

			IHCDescriptorWriter(*textureDescriptorSetLayout, *localDescriptorPool)
				.WriteImage(binding, &imageInfo)// Sampler
				.Build(descriptor);

			descriptorSetsForTexture.push_back(descriptor);
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
			availableTextureDescriptorSets.push(descriptor);
		}
		texture->SetDescriptorSets({});
	}

	void IHCDescriptorManager::AllocateSkeletalDescriptorSetForAnimator(Animator* animator)
	{
		// check if already allocate descriptorsets for the texture;
		if (!animator->GetDescriptorSets().empty())
		{
			assert("Duplicated animator");
		}
		// allocate MAX_FRAMES_IN_FLIGHT descriptorsets for 1 animator
		std::vector<VkDescriptorSet> descriptorSetsForAnimator;
		std::vector<IHCBuffer*> skeletalUBOsForAnimator;

		for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (availableSkeletalDescriptorSets.empty())
			{
				assert("No available descriptor sets for allocation. Check if exceed poolsize");
			}
			VkDescriptorSet descriptor = availableSkeletalDescriptorSets.top();
			availableSkeletalDescriptorSets.pop();

			if (availableSkeletalUBOs.empty())
			{
				assert("No buffers for allocation. Check if exceed poolsize");
			}
			IHCBuffer* buffer = availableSkeletalUBOs.top();
			availableSkeletalUBOs.pop();


			auto bufferInfo = buffer->GetDescriptorInfo();
			IHCDescriptorWriter(*skeletalDescriptorSetLayout, *localDescriptorPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(descriptor);

			descriptorSetsForAnimator.push_back(descriptor);
			skeletalUBOsForAnimator.push_back(buffer);
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
			availableSkeletalDescriptorSets.push(descriptor);
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
			availableSkeletalUBOs.push(buffer);
		}
		animator->SetBuffers({});
	}
}
