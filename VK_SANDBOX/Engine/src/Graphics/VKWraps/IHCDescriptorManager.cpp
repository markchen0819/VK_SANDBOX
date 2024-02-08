#include "../../pch.h"
#include "IHCDescriptorManager.h"

#include "IHCSwapChain.h"
#include "IHCDevice.h"
#include "IHCBuffer.h"
#include "VKHelpers.h"

#include "IHCTexture.h"
#include "../Animation/Animator.h"
#include "../Animation/InverseKinematicsSolver.h"
#include "../Particle/ComputeParticleUniformBufferObject.h"
#include "../../Core/Scene/Components/ComputeParticleComponent.h"
#include "../Grass/ComputeGrassUniformBufferObject.h"
#include "../../Core/Scene/Components/ComputeGrassComponent.h"


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

		computeParticleDescriptorWrap = std::make_unique<ComputeParticleDescriptorWrap>(ihcDevice);
		computeParticleDescriptorWrap->Setup();
		computeGrassDescriptorWrap = std::make_unique<ComputeGrassDescriptorWrap>(ihcDevice);
		computeGrassDescriptorWrap->Setup();
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
			textureDescriptorWrap->OverwriteBindings(targetDesciptorSet);

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
			skeletalDescriptorWrap->OverwriteBindings(targetDescriptorSet);
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
			skeletalDescriptorWrap->OverwriteBindings(targetDescriptorSet);
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

	void IHCDescriptorManager::AllocateComputeParticleDescriptorSet(Component::ComputeParticleComponent* computeParticle)
	{
		std::vector<IHCBuffer*> computeParticleUniformBuffers;
		std::vector<VkDescriptorSet> computeDescriptorSets;
		const int maxParticleCount = computeParticle->GetMaxParticleCount();

		// Get Shader Storage buffers from component itself
		std::vector<IHCBuffer*> shaderStorageBuffers = computeParticle->GetSSBO();

		// Get Uniform buffers from DescriptorWrap
		for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto buffer = computeParticleDescriptorWrap->GetAvailableComputeParticleUBO();
			computeParticleUniformBuffers.push_back(buffer);
		}

		// Allocate descriptorSets from  DescriptorWrap and bind to buffer
		// Allocate multiple descriptor sets of the same layout in a single call
		auto descriptorSetLayout = computeParticleDescriptorWrap->GetDescriptorSetLayout();
		auto descriptorPool = computeParticleDescriptorWrap->GetPool();

		std::vector<VkDescriptorSetLayout> layouts(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();
		computeDescriptorSets.resize(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(ihcDevice.GetDevice(), &allocInfo, computeDescriptorSets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		// Update each allocated descriptor set with the actual resources (buffers) they will reference
		for (size_t i = 0; i < Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			// DescriptorSet: computeDescriptorSets[i]
			// Bind resources: uniformBuffers[i] shaderStorageBuffers[i]  shaderStorageBuffers[i-1] 

			VkDescriptorBufferInfo uniformBufferInfo{};
			uniformBufferInfo.buffer = computeParticleUniformBuffers[i]->GetBuffer();
			uniformBufferInfo.offset = 0;
			uniformBufferInfo.range = sizeof(Graphics::ComputeParticleUniformBufferObject);

			VkDescriptorBufferInfo storageBufferInfoLastFrame{};
			storageBufferInfoLastFrame.buffer = shaderStorageBuffers[(i - 1) % Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT]->GetBuffer();
			storageBufferInfoLastFrame.offset = 0;
			storageBufferInfoLastFrame.range = sizeof(Graphics::Particle) * maxParticleCount;

			VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
			storageBufferInfoCurrentFrame.buffer = shaderStorageBuffers[i]->GetBuffer();
			storageBufferInfoCurrentFrame.offset = 0;
			storageBufferInfoCurrentFrame.range = sizeof(Graphics::Particle) * maxParticleCount;

			std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
			// Configure the write operation for the uniform buffer at binding 0
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = computeDescriptorSets[i]; // Destination descriptor set
			descriptorWrites[0].dstBinding = 0; // Binding 0 in the set
			descriptorWrites[0].dstArrayElement = 0; // First element in the array (if it was an array)
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1; // We're updating one descriptor
			descriptorWrites[0].pBufferInfo = &uniformBufferInfo; // Pointer to the buffer info

			// Configure the write operation for the storage buffer at binding 1 (last frame's data)
			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = computeDescriptorSets[i];
			descriptorWrites[1].dstBinding = 1; // Binding 1 in the set
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = &storageBufferInfoLastFrame;

			// Configure the write operation for the storage buffer at binding 2 (current frame's data)
			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = computeDescriptorSets[i];
			descriptorWrites[2].dstBinding = 2; // Binding 2 in the set
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pBufferInfo = &storageBufferInfoCurrentFrame;

			vkUpdateDescriptorSets(ihcDevice.GetDevice(),
				3, descriptorWrites.data(),
				0, nullptr);
		}
		computeParticle->SetDescriptorSets(computeDescriptorSets);
		computeParticle->SetUniformBuffers(computeParticleUniformBuffers);
		// no need to set SSBO, what we did here is we get resource from computeParticle and bind
	}

	void IHCDescriptorManager::DeallocateComputeParticleDescriptorSet(Component::ComputeParticleComponent* computeParticle)
	{
		// All submitted commands that refer to sampler must have completed execution
		vkDeviceWaitIdle(ihcDevice.GetDevice());

		const std::vector<VkDescriptorSet>& IKDescriptorSets = computeParticle->GetDescriptorSets();
		if (IKDescriptorSets.empty())
		{
			assert("Clearing descriptor sets were not allocated, shouldnt be here ");
		}
		// Push back each descriptor set to the available pool 
		for (VkDescriptorSet descriptor : IKDescriptorSets)
		{
			computeParticleDescriptorWrap->FreeDescriptorSet(descriptor);
		}
		computeParticle->SetDescriptorSets({});

		const std::vector<IHCBuffer*>& buffers = computeParticle->GetUnformBuffers();
		if (buffers.empty())
		{
			assert("Clearing buffers were not allocated, shouldnt be here ");
		}
		// Push back each buffer  to the available pool 
		for (IHCBuffer* buffer : buffers)
		{
			computeParticleDescriptorWrap->ReleaseComputeParticleUBO(buffer);
		}
		computeParticle->SetUniformBuffers({});
	}

	void IHCDescriptorManager::AllocateComputeGrassDescriptorSet(Component::ComputeGrassComponent* computeGrass)
	{
		std::vector<IHCBuffer*> computeGrassUniformBuffers;
		std::vector<VkDescriptorSet> computeDescriptorSets;
		const int maxGrassBladeCount = computeGrass->GetMaxGrassBladeCount();

		// Get Shader Storage buffers from component itself
		std::vector<IHCBuffer*> shaderStorageBuffers = computeGrass->GetSSBO();

		// Get Uniform buffers from DescriptorWrap
		for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto buffer = computeGrassDescriptorWrap->GetAvailableComputeGrassUBO();
			computeGrassUniformBuffers.push_back(buffer);
		}

		// Allocate descriptorSets from  DescriptorWrap and bind to buffer
		// Allocate multiple descriptor sets of the same layout in a single call
		auto descriptorSetLayout = computeGrassDescriptorWrap->GetDescriptorSetLayout();
		auto descriptorPool = computeGrassDescriptorWrap->GetPool();

		std::vector<VkDescriptorSetLayout> layouts(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();
		computeDescriptorSets.resize(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(ihcDevice.GetDevice(), &allocInfo, computeDescriptorSets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		// Update each allocated descriptor set with the actual resources (buffers) they will reference
		for (size_t i = 0; i < Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			// DescriptorSet: computeDescriptorSets[i]
			// Bind resources: uniformBuffers[i] shaderStorageBuffers[i]  shaderStorageBuffers[i-1] 

			VkDescriptorBufferInfo uniformBufferInfo{};
			uniformBufferInfo.buffer = computeGrassUniformBuffers[i]->GetBuffer();
			uniformBufferInfo.offset = 0;
			uniformBufferInfo.range = sizeof(Graphics::ComputeGrassUniformBufferObject);

			VkDescriptorBufferInfo storageBufferInfoLastFrame{};
			storageBufferInfoLastFrame.buffer = shaderStorageBuffers[(i - 1) % Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT]->GetBuffer();
			storageBufferInfoLastFrame.offset = 0;
			storageBufferInfoLastFrame.range = sizeof(GrassBlade) * maxGrassBladeCount;

			VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
			storageBufferInfoCurrentFrame.buffer = shaderStorageBuffers[i]->GetBuffer();
			storageBufferInfoCurrentFrame.offset = 0;
			storageBufferInfoCurrentFrame.range = sizeof(GrassBlade) * maxGrassBladeCount;

			std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
			// Configure the write operation for the uniform buffer at binding 0
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = computeDescriptorSets[i]; // Destination descriptor set
			descriptorWrites[0].dstBinding = 0; // Binding 0 in the set
			descriptorWrites[0].dstArrayElement = 0; // First element in the array (if it was an array)
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1; // We're updating one descriptor
			descriptorWrites[0].pBufferInfo = &uniformBufferInfo; // Pointer to the buffer info

			// Configure the write operation for the storage buffer at binding 1 (last frame's data)
			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = computeDescriptorSets[i];
			descriptorWrites[1].dstBinding = 1; // Binding 1 in the set
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = &storageBufferInfoLastFrame;

			// Configure the write operation for the storage buffer at binding 2 (current frame's data)
			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = computeDescriptorSets[i];
			descriptorWrites[2].dstBinding = 2; // Binding 2 in the set
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pBufferInfo = &storageBufferInfoCurrentFrame;

			vkUpdateDescriptorSets(ihcDevice.GetDevice(),
				3, descriptorWrites.data(),
				0, nullptr);
		}
		computeGrass->SetDescriptorSets(computeDescriptorSets);
		computeGrass->SetUniformBuffers(computeGrassUniformBuffers);
		// no need to set SSBO, what we did here is we get resource from computeParticle and bind
	}

	void IHCDescriptorManager::DeallocateComputeGrassDescriptorSet(Component::ComputeGrassComponent* computeGrass)
	{
		// All submitted commands that refer to sampler must have completed execution
		vkDeviceWaitIdle(ihcDevice.GetDevice());

		const std::vector<VkDescriptorSet>& IKDescriptorSets = computeGrass->GetDescriptorSets();
		if (IKDescriptorSets.empty())
		{
			assert("Clearing descriptor sets were not allocated, shouldnt be here ");
		}
		// Push back each descriptor set to the available pool 
		for (VkDescriptorSet descriptor : IKDescriptorSets)
		{
			computeGrassDescriptorWrap->FreeDescriptorSet(descriptor);
		}
		computeGrass->SetDescriptorSets({});

		const std::vector<IHCBuffer*>& buffers = computeGrass->GetUnformBuffers();
		if (buffers.empty())
		{
			assert("Clearing buffers were not allocated, shouldnt be here ");
		}
		// Push back each buffer  to the available pool 
		for (IHCBuffer* buffer : buffers)
		{
			computeGrassDescriptorWrap->ReleaseComputeGrassUBO(buffer);
		}
		computeGrass->SetUniformBuffers({});
	}
}
