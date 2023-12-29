#include "../../../pch.h"
#include "ComputeParticleComponent.h"

#include "../../Locator/GraphicsManagerLocator.h"
#include "../../../Graphics/VKWraps/IHCSwapChain.h"
#include "../../../Graphics/VKWraps/IHCBuffer.h"
#include "../../../Graphics/VKWraps/IHCDevice.h"
#include "../../../Graphics/Particle/ComputeParticleUniformBufferObject.h"
#include "../../../Graphics/Particle/Particle.h"


#include <random>

IHCEngine::Component::ComputeParticleComponent::ComputeParticleComponent()
	:Component(ComponentType::ComputeParticle)
{
	initParticles();
	createVulkanResources();
}

void IHCEngine::Component::ComputeParticleComponent::initParticles()
{
	particles.resize(PARTICLE_COUNT);

	const int HEIGHT = 1280;
	const int WIDTH = 720;

	//// Initialize particles
	std::default_random_engine rndEngine((unsigned)time(nullptr));
	std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

	for (auto& particle : particles)
	{
		float r = 0.25f * sqrt(rndDist(rndEngine));
		float theta = rndDist(rndEngine) * 2.0f * 3.14159265358979323846f;
		float x = r * cos(theta) * HEIGHT / WIDTH;
		float y = r * sin(theta);
		particle.position = glm::vec2(x, y);
		particle.velocity = glm::normalize(glm::vec2(x, y)) * 0.00025f;
		particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
	}
}


void IHCEngine::Component::ComputeParticleComponent::createVulkanResources()
{
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	vkDeviceWaitIdle(graphicsManager->GetIHCDevice()->GetDevice());

	createUniformBuffers();
	createShaderStorageBuffers();
	createComputeDescriptorSets();
}

void IHCEngine::Component::ComputeParticleComponent::createShaderStorageBuffers()
{
	shaderStorageBuffers.resize(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
	VkDeviceSize bufferSize = sizeof(Graphics::Particle) * PARTICLE_COUNT;

	// temporary buffer accessed by CPU and GPU
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	uint32_t instanceSize = sizeof(Graphics::Particle);
	uint32_t instanceCount = PARTICLE_COUNT;
	Graphics::IHCBuffer stagingBuffer
	{
		*graphicsManager->GetIHCDevice(),
		instanceSize,
		instanceCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	};
	// map temporary buffer memory to CPU address space
	// (able to write/read on CPU )
	// Copy particle data to staging buffer
	stagingBuffer.Map();
	stagingBuffer.WriteToBuffer((void*)particles.data());
	stagingBuffer.Unmap(); //also handled automatically in destructor
	// Copy initial particle data to all storage buffers (GPU)
	for (size_t i = 0; i < Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
	{
		shaderStorageBuffers[i] = std::make_unique<Graphics::IHCBuffer>(
			*graphicsManager->GetIHCDevice(),
			instanceSize,
			instanceCount,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		graphicsManager->GetIHCDevice()->CopyBuffer(stagingBuffer.GetBuffer(), shaderStorageBuffers[i]->GetBuffer(), bufferSize);
	}
}

void IHCEngine::Component::ComputeParticleComponent::createUniformBuffers()
{
	// Uniform buffers
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	computeParticleUniformBuffers.resize(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < computeParticleUniformBuffers.size(); i++)
	{
		computeParticleUniformBuffers[i] = std::make_unique<Graphics::IHCBuffer>
			(
				*graphicsManager->GetIHCDevice(),
				sizeof(Graphics::ComputeParticleUniformBufferObject),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
		computeParticleUniformBuffers[i]->Map(); // persistent mapping
	}

}

void IHCEngine::Component::ComputeParticleComponent::createComputeDescriptorSets()
{
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	// DescriptorSets
	// Allocate multiple descriptor sets of the same layout in a single call
	std::vector<VkDescriptorSetLayout> layouts(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();
	computeDescriptorSets.resize(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(graphicsManager->GetIHCDevice()->GetDevice(), &allocInfo, computeDescriptorSets.data()) != VK_SUCCESS)
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
		storageBufferInfoLastFrame.range = sizeof(Graphics::Particle) * PARTICLE_COUNT;

		VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
		storageBufferInfoCurrentFrame.buffer = shaderStorageBuffers[i]->GetBuffer();
		storageBufferInfoCurrentFrame.offset = 0;
		storageBufferInfoCurrentFrame.range = sizeof(Graphics::Particle) * PARTICLE_COUNT;

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

		vkUpdateDescriptorSets(graphicsManager->GetIHCDevice()->GetDevice(),
			3, descriptorWrites.data(),
			0, nullptr);
	}
}

void IHCEngine::Component::ComputeParticleComponent::Attach()
{
}

void IHCEngine::Component::ComputeParticleComponent::Remove()
{

}
