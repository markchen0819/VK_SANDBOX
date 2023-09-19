#include "../../pch.h"
#include "IHCDescriptorManager.h"

#include "IHCSwapChain.h"
#include "IHCBuffer.h"
#include "IHCDescriptors.h"
#include "IHCDevice.h"
#include "IHCTexture.h"
#include "VKHelpers.h"

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
		// skeletalDescriptorSetLayout set 2, Binding0, UNIFORM_BUFFER
		std::vector<VkDescriptorSetLayout> layouts {
			globalDescriptorSetLayout->GetDescriptorSetLayout(),
			textureDescriptorSetLayout->GetDescriptorSetLayout(),
			skeletalDescriptorSetLayout->GetDescriptorSetLayout(),
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
		for (int i = 0; i < globalUBOs.size(); i++)
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
		if (textureToDescriptorSetsMap.find(assetName) != textureToDescriptorSetsMap.end())
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

			IHCDescriptorWriter(*textureDescriptorSetLayout, *localDescriptorPool)
				.WriteImage(0, &imageInfo)// Sampler
				.Build(descriptor);

			descriptorSetsForTexture.push_back(descriptor);
		}
		// Add the collection of descriptor sets to the map.
		textureToDescriptorSetsMap[assetName] = descriptorSetsForTexture;
	}
	void IHCDescriptorManager::DeallocateTextureDescriptorSetForTexture(std::string assetName)
	{
		// All submitted commands that refer to sampler must have completed execution
		vkDeviceWaitIdle(ihcDevice.GetDevice());
		// Check if the textureID exists in the map.
		auto it = textureToDescriptorSetsMap.find(assetName);
		if (it == textureToDescriptorSetsMap.end())
		{
			std::cerr << "Texture not found in descriptor map." << std::endl;
			return;
		}
		// Push all its descriptor sets as available.
		for (VkDescriptorSet descSet : it->second)
		{
			availableTextureDescriptorSets.push(descSet);
		}
		// Remove the textureID from the map.
		textureToDescriptorSetsMap.erase(it);
	}

	void IHCDescriptorManager::AllocateSkeletalDescriptorSetForAnimator(Animator* animator)
	{
		// check if already allocate descriptorsets for the texture;
		if (animatorToDescriptorSetsMap.find(animator) != animatorToDescriptorSetsMap.end())
		{
			assert("Duplicated animator");
		}
		// allocate MAX_FRAMES_IN_FLIGHT descriptorsets for 1 animator
		std::vector<VkDescriptorSet> descriptorSetsForAnimator;
		for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (availableSkeletalDescriptorSets.empty())
			{
				assert("No available descriptor sets for allocation. Check if exceed poolsize");
			}

			VkDescriptorSet descriptor = availableSkeletalDescriptorSets.top();
			availableSkeletalDescriptorSets.pop();

			auto bufferInfo =skeletalUBOs[i]->GetDescriptorInfo();
			IHCDescriptorWriter(*skeletalDescriptorSetLayout, *localDescriptorPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(descriptor);

			descriptorSetsForAnimator.push_back(descriptor);
		}
		// Add the collection of descriptor sets to the map.
		animatorToDescriptorSetsMap[animator] = descriptorSetsForAnimator;
	}
	void IHCDescriptorManager::DeallocateSkeletalDescriptorSetForAnimator(Animator* animator)
	{
		// All submitted commands that refer to sampler must have completed execution
		vkDeviceWaitIdle(ihcDevice.GetDevice());
		// Check if the animator exists in the map.
		auto it = animatorToDescriptorSetsMap.find(animator);
		if (it == animatorToDescriptorSetsMap.end())
		{
			std::cerr << "Animator not found in descriptor map." << std::endl;
			return;
		}
		// Push all its descriptor sets as available.
		for (VkDescriptorSet descSet : it->second)
		{
			availableSkeletalDescriptorSets.push(descSet);
		}
		// Remove the animator  from the map.
		animatorToDescriptorSetsMap.erase(it);
	}
}
