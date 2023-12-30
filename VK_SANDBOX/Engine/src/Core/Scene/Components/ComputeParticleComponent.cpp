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
}

void IHCEngine::Component::ComputeParticleComponent::initParticles()
{
	particles.resize(maxParticleCount);

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
	auto& graphicsAssetCreator = graphicsManager->GetGraphicsAssetCreator();
	vkDeviceWaitIdle(graphicsManager->GetIHCDevice()->GetDevice());

	createShaderStorageBuffers();
	graphicsAssetCreator.CreateParticleData(this); // allocateUniformBuffersAndDescriptorSets
}
void IHCEngine::Component::ComputeParticleComponent::destroyVulkanResources()
{
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	graphicsAssetCreator.DestroyParticleData(this); // deallocateUniformBuffersAndDescriptorSets
	for (size_t i = 0; i < Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
	{
		shaderStorageBuffers[i] = nullptr;
	}
}

void IHCEngine::Component::ComputeParticleComponent::createShaderStorageBuffers()
{
	shaderStorageBuffers.resize(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
	VkDeviceSize bufferSize = sizeof(Graphics::Particle) * maxParticleCount;

	// temporary buffer accessed by CPU and GPU
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	uint32_t instanceSize = sizeof(Graphics::Particle);
	uint32_t instanceCount = maxParticleCount;
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

void IHCEngine::Component::ComputeParticleComponent::Attach()
{
	createVulkanResources();
}

void IHCEngine::Component::ComputeParticleComponent::Remove()
{
	destroyVulkanResources();
}
