#include "../../../pch.h"
#include "ComputeParticleDescriptorWrap.h"

#include "../IHCSwapChain.h"
#include "../IHCBuffer.h"
#include "../IHCDevice.h"
#include "../VKHelpers.h"
#include "../../Particle/Particle.h"
#include "../../Particle/ComputeParticleUniformBufferObject.h"

namespace IHCEngine::Graphics
{
	ComputeParticleDescriptorWrap::ComputeParticleDescriptorWrap(IHCDevice& device)
		: IHCDescriptorWrap(device)
	{
	}

	ComputeParticleDescriptorWrap::~ComputeParticleDescriptorWrap()
	{
	}

	void ComputeParticleDescriptorWrap::Setup()
	{
		// Sum of sets needed for all particle systems and frames
		SetMaxSets(PARTICLE_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			PARTICLE_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 
			2 * PARTICLE_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		//AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		//	TEXTURE_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT); //TO:DO textures
		InitializePool();

		// Accessible by the compute stage
		// uniform buffer: feed in particle data
		// storage buffer: read/write for compute shader
		AddLayoutBinding(
			0,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_SHADER_STAGE_COMPUTE_BIT,
			1
		);
		AddLayoutBinding(
			1,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_SHADER_STAGE_COMPUTE_BIT,
			1
		);
		AddLayoutBinding(
			2,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_SHADER_STAGE_COMPUTE_BIT,
			1
		);
		CreateLayout();



		CreateResources();
		AllocAndBindDescriptorSets();
	}

	void ComputeParticleDescriptorWrap::CreateResources()
	{
		//std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> computeParticleUniformBuffers;

		//computeParticleUniformBuffers.resize(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		//for (int i = 0; i < computeParticleUniformBuffers.size(); i++)
		//{
		//	computeParticleUniformBuffers[i] = std::make_unique<IHCBuffer>
		//		(
		//			ihcDevice,
		//			sizeof(ComputeParticleUniformBufferObject),
		//			1,
		//			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		//			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		//		);
		//	computeParticleUniformBuffers[i]->Map(); // persistent mapping
		//}

	}

	void ComputeParticleDescriptorWrap::AllocAndBindDescriptorSets()
	{
		////!! TO:DO move this to while particle component is created
		//// Temp
		//int PARTICLE_COUNT = 65536;// particle component contains how many particle
		//std::vector<VkDescriptorSet> computeDescriptorSets;
		//std::vector<VkBuffer> uniformBuffers;
		//std::vector<VkBuffer> shaderStorageBuffers;

		//// Allocate multiple descriptor sets of the same layout in a single call
		//std::vector<VkDescriptorSetLayout> layouts(IHCSwapChain::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		//VkDescriptorSetAllocateInfo allocInfo{};
		//allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		//allocInfo.descriptorPool = descriptorPool;
		//allocInfo.descriptorSetCount = static_cast<uint32_t>(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		//allocInfo.pSetLayouts = layouts.data();
		//computeDescriptorSets.resize(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		//if (vkAllocateDescriptorSets(ihcDevice.GetDevice(), &allocInfo, computeDescriptorSets.data()) != VK_SUCCESS)
		//{
		//	throw std::runtime_error("failed to allocate descriptor sets!");
		//}

		//// Update each allocated descriptor set with the actual resources (buffers) they will reference
		//for (size_t i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		//{
		//	// DescriptorSet: computeDescriptorSets[i]
		//	// Bind resources: uniformBuffers[i] shaderStorageBuffers[i]  shaderStorageBuffers[i-1] 

		//	VkDescriptorBufferInfo uniformBufferInfo{};
		//	uniformBufferInfo.buffer = uniformBuffers[i]; 
		//	uniformBufferInfo.offset = 0;
		//	uniformBufferInfo.range = sizeof(ComputeParticleUniformBufferObject);

		//	VkDescriptorBufferInfo storageBufferInfoLastFrame{};
		//	storageBufferInfoLastFrame.buffer = shaderStorageBuffers[(i - 1) % IHCSwapChain::MAX_FRAMES_IN_FLIGHT];
		//	storageBufferInfoLastFrame.offset = 0;
		//	storageBufferInfoLastFrame.range = sizeof(Particle) * PARTICLE_COUNT;

		//	VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
		//	storageBufferInfoCurrentFrame.buffer = shaderStorageBuffers[i];
		//	storageBufferInfoCurrentFrame.offset = 0;
		//	storageBufferInfoCurrentFrame.range = sizeof(Particle) * PARTICLE_COUNT;

		//	std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
		//	// Configure the write operation for the uniform buffer at binding 0
		//	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//	descriptorWrites[0].dstSet = computeDescriptorSets[i]; // Destination descriptor set
		//	descriptorWrites[0].dstBinding = 0; // Binding 0 in the set
		//	descriptorWrites[0].dstArrayElement = 0; // First element in the array (if it was an array)
		//	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//	descriptorWrites[0].descriptorCount = 1; // We're updating one descriptor
		//	descriptorWrites[0].pBufferInfo = &uniformBufferInfo; // Pointer to the buffer info

		//	// Configure the write operation for the storage buffer at binding 1 (last frame's data)
		//	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//	descriptorWrites[1].dstSet = computeDescriptorSets[i];
		//	descriptorWrites[1].dstBinding = 1; // Binding 1 in the set
		//	descriptorWrites[1].dstArrayElement = 0;
		//	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		//	descriptorWrites[1].descriptorCount = 1;
		//	descriptorWrites[1].pBufferInfo = &storageBufferInfoLastFrame;

		//	// Configure the write operation for the storage buffer at binding 2 (current frame's data)
		//	descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//	descriptorWrites[2].dstSet = computeDescriptorSets[i];
		//	descriptorWrites[2].dstBinding = 2; // Binding 2 in the set
		//	descriptorWrites[2].dstArrayElement = 0;
		//	descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		//	descriptorWrites[2].descriptorCount = 1;
		//	descriptorWrites[2].pBufferInfo = &storageBufferInfoCurrentFrame;

		//	vkUpdateDescriptorSets(ihcDevice.GetDevice(),
		//		3, descriptorWrites.data(),
		//		0, nullptr);
		//}
	}
}
