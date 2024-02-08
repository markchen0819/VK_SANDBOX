#include "../../../pch.h"
#include "ComputeGrassComponent.h"

#include "../../Locator/GraphicsManagerLocator.h"
#include "../../Locator/SceneManagerLocator.h"
#include "../../../Core/Time/Time.h"
#include "../../../Graphics/VKWraps/IHCSwapChain.h"
#include "../../../Graphics/VKWraps/IHCBuffer.h"
#include "../../../Graphics/VKWraps/IHCDevice.h"
#include "../../../Graphics/Grass/ComputeGrassUniformBufferObject.h"
#include "../../../Graphics/Grass/GrassBlade.h"


#include <random>
#include <omp.h>

IHCEngine::Component::ComputeGrassComponent::ComputeGrassComponent()
	:Component(ComponentType::ComputeGrass)
{

}

void IHCEngine::Component::ComputeGrassComponent::Compute(Graphics::FrameInfo& frameInfo)
{
	float dt = IHCEngine::Core::Time::GetDeltaTime();
	lastFrameTime += dt;

	ubo.deltaTime = dt;
	ubo.accumulatedTime = lastFrameTime;


	computeGrassUniformBuffers[frameInfo.frameIndex]->WriteToBuffer(&ubo);
	//computeGrassUniformBuffers[frameInfo.frameIndex]->Flush(); // Manual flush, can comment out if using VK_MEMORY_PROPERTY_HOST_COHERENT_BIT

	// Dispatch
	vkCmdDispatch(frameInfo.commandBuffer, grassBladeCount / 256, 1, 1);
}

void IHCEngine::Component::ComputeGrassComponent::Draw(Graphics::FrameInfo& frameInfo)
{
	// Draw Point
	//VkBuffer shaderStorageBuffers[] = { GetSSBO()[frameInfo.frameIndex]->GetBuffer() };
	//VkDeviceSize offsets[] = { 0 };
	//vkCmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, shaderStorageBuffers, offsets);
	//vkCmdDraw(frameInfo.commandBuffer, grassBladeCount, 1, 0, 0);
}

void IHCEngine::Component::ComputeGrassComponent::initGrassBlades()
{
	grassBlades.resize(maxGrassBladeCount);

	// Initialize grassBlades
	std::default_random_engine rndEngine((unsigned)time(nullptr));
	std::uniform_real_distribution<float> colorDistribution(0.0f, 1.0f);
	std::uniform_real_distribution<float> positionDistribution(-10.0f, 10.0f);

	for (int i = 0; i < grassBlades.size(); ++i)
	{
		auto& particle = grassBlades[i];

		float x = positionDistribution(rndEngine);
		float y = positionDistribution(rndEngine);
		float z = positionDistribution(rndEngine);

		particle.position = glm::vec4(x, 0, z, 0);
		particle.rotation = glm::vec4(1, 0, 0, 0);
		particle.scale = glm::vec4(0.03, 0.03, 0.03, 0);
		particle.color = glm::vec4(colorDistribution(rndEngine), colorDistribution(rndEngine), 0, 0.5f);
	}
}


void IHCEngine::Component::ComputeGrassComponent::createVulkanResources()
{
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	auto& graphicsAssetCreator = graphicsManager->GetGraphicsAssetCreator();
	vkDeviceWaitIdle(graphicsManager->GetIHCDevice()->GetDevice());

	createShaderStorageBuffers();
	graphicsAssetCreator.CreateGrassData(this); // allocateUniformBuffersAndDescriptorSets
}
void IHCEngine::Component::ComputeGrassComponent::destroyVulkanResources()
{
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	graphicsAssetCreator.DestroyGrassData(this); // deallocateUniformBuffersAndDescriptorSets
	for (size_t i = 0; i < Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
	{
		shaderStorageBuffers[i] = nullptr;
	}
}

void IHCEngine::Component::ComputeGrassComponent::createShaderStorageBuffers()
{
	shaderStorageBuffers.resize(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
	VkDeviceSize bufferSize = sizeof(Graphics::GrassBlade) * maxGrassBladeCount;

	// temporary buffer accessed by CPU and GPU
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	uint32_t instanceSize = sizeof(Graphics::GrassBlade);
	uint32_t instanceCount = maxGrassBladeCount;
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
	stagingBuffer.WriteToBuffer((void*)grassBlades.data());
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

void IHCEngine::Component::ComputeGrassComponent::Attach()
{
	initGrassBlades();
	createVulkanResources();
	Core::GraphicsManagerLocator::GetGraphicsManager()->GetParticleSystem().AddGameObject(this->gameObject, Graphics::PipelineType::COMPUTEGRASS);
}

void IHCEngine::Component::ComputeGrassComponent::Remove()
{
	destroyVulkanResources();
	Core::GraphicsManagerLocator::GetGraphicsManager()->GetParticleSystem().RemoveGameObject(this->gameObject, Graphics::PipelineType::COMPUTEGRASS);
}
