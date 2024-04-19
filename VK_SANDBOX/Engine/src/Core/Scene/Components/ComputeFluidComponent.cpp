#include "../../../pch.h"
#include "ComputeFluidComponent.h"

#include "../../Locator/GraphicsManagerLocator.h"
#include "../../Locator/SceneManagerLocator.h"
#include "../../../Graphics/VKWraps/IHCSwapChain.h"
#include "../../../Graphics/VKWraps/IHCBuffer.h"
#include "../../../Graphics/VKWraps/IHCDevice.h"

#include "../../../Core/Time/Time.h"

#include <random>
#include <omp.h>

IHCEngine::Component::ComputeFluidComponent::ComputeFluidComponent()
	:Component(ComponentType::ComputeParticle)
{
}

void IHCEngine::Component::ComputeFluidComponent::Compute(Graphics::FrameInfo& frameInfo)
{
	float dt = IHCEngine::Core::Time::GetDeltaTime();
	ubo.deltaTime = 1.0f / 120.0f;// dt;
	ubo.accumulatedTime = IHCEngine::Core::Time::GetElapsedDeltaTime();
	ubo.particleCount = particleCount;
	ubo.gasConstant = gasConstant;
	ubo.restDensity = restDensity;
	ubo.viscosity = viscosity;
	ubo.radius = radius;
	ubo.maxBound = maxBound;
	ubo.minBound = minBound;
	ubo.interactionSpherePos = interactionSpherePos;

	computeParticleUniformBuffers[frameInfo.frameIndex]->WriteToBuffer(&ubo);
	//computeParticleUniformBuffers[frameInfo.frameIndex]->Flush(); // Manual flush, can comment out if using VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	// Move Dispatch into Pipeline (since need multiple compute shader calls)
}

void IHCEngine::Component::ComputeFluidComponent::Draw(Graphics::FrameInfo& frameInfo)
{
	// Check in ComputeFluidPipeline::Render

	////Draw Point skipping vertex input
	//vkCmdDraw(frameInfo.commandBuffer, 1, particleCount, 0, 0);
}

void IHCEngine::Component::ComputeFluidComponent::initParticles()
{
	particles.resize(maxParticleCount);
	spacialEntries.resize(maxParticleCount);
	spacialLookups.resize(maxParticleCount);

	// Initialize particles
	std::default_random_engine rndEngine((unsigned)time(nullptr));
	std::uniform_real_distribution<float> colorDistribution(0.0f, 1.0f);
	std::uniform_real_distribution<float> xzDistribution(-5.0f, 5.0f);
	std::uniform_real_distribution<float> yDistribution(10.0f, 15.0f);
	std::uniform_real_distribution<float> velocityDistribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> posDistribution(-1.0f, 1.0f);
	// auto start = std::chrono::high_resolution_clock::now();
	//#pragma omp parallel for
	// break for loop into possible parallel threads, doesn't speed up after testing, recheck in future

	int n = 16;
	float xMin = -5.0f, xMax = 5.0f;
	float yMin = 2.0f, yMax = 12.0f;
	float zMin = -5.0f, zMax = 5.0f;
	float xSpacing = (xMax - xMin) / (n - 1);
	float ySpacing = (yMax - yMin) / (n - 1);
	float zSpacing = (zMax - zMin) / (n - 1);
	int index = 0; // Index to keep track of the particle number
	for (int i = 0; i < n; ++i) 
	{
		for (int j = 0; j < n; ++j) 
		{
			for (int k = 0; k < n; ++k) 
			{
				float x = xMin + i * xSpacing;
				float y = yMin + j * ySpacing;
				float z = zMin + k * zSpacing;
				x += posDistribution(rndEngine);
				z+= posDistribution(rndEngine);
				particles[index].position = glm::vec4(x, y, z, 1.0f);
				particles[index].predictPosition = particles[index].position;
				particles[index].velocity = glm::vec4(0, 0, 0, 0); // Assuming initial velocity is zero
				particles[index].color = glm::vec4(colorDistribution(rndEngine), 0, colorDistribution(rndEngine), 0.5f);
				spacialLookups[index].index = static_cast<glm::uint>(index);
				//spacialEntries[index].index = index + 1;
				//spacialEntries[index].hash = index + 2;
				//spacialEntries[index].key = index + 3;
				++index;
			}
		}
	}

	//for (int i = 0; i < particles.size(); ++i)
	//{
	//	auto& particle = particles[i];

	//	float x = xzDistribution(rndEngine);
	//	float y = yDistribution(rndEngine);
	//	float z = xzDistribution(rndEngine);
	//	particle.position = glm::vec4(x, y, z, 0);
	//	particle.predictPosition = particle.position;
	//	//float v1 = 0.1f * velocityDistribution(rndEngine);
	//	//float v2 = 0.1f * velocityDistribution(rndEngine);
	//	//float v3 = 0.1f * velocityDistribution(rndEngine);
	//	//particle.velocity = glm::vec4(v1, v2, v3, 0);
	//	particle.velocity = glm::vec4(0, 0, 0, 0);
	//	particle.color = glm::vec4(colorDistribution(rndEngine), 0 , colorDistribution(rndEngine), 0.5f);
	//	particle.force = glm::vec4(0.0f);
	//}

	//auto end = std::chrono::high_resolution_clock::now();
	//std::chrono::duration<double, std::milli> duration = end - start;
	//std::cout << "Execution time without parallelization: " << duration.count() << " ms\n";
}

void IHCEngine::Component::ComputeFluidComponent::createVulkanResources()
{
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	auto& graphicsAssetCreator = graphicsManager->GetGraphicsAssetCreator();
	vkDeviceWaitIdle(graphicsManager->GetIHCDevice()->GetDevice());

	createShaderStorageBuffers();
	createSpacialSSBOs();
	graphicsAssetCreator.CreateFluidData(this); // allocateUniformBuffersAndDescriptorSets
}
void IHCEngine::Component::ComputeFluidComponent::destroyVulkanResources()
{
	auto& graphicsAssetCreator = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager()->GetGraphicsAssetCreator();

	graphicsAssetCreator.DestroyFluidData(this); // deallocateUniformBuffersAndDescriptorSets
	for (size_t i = 0; i < Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
	{
		shaderStorageBuffers[i] = nullptr;
		spacialEntrySSBOs[i] = nullptr;
		spacialLookupSSBOs[i] = nullptr;
	}
}
void IHCEngine::Component::ComputeFluidComponent::createShaderStorageBuffers()
{
	shaderStorageBuffers.resize(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
	VkDeviceSize bufferSize = sizeof(Graphics::FluidParticle) * maxParticleCount;

	// temporary buffer accessed by CPU and GPU
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	uint32_t instanceSize = sizeof(Graphics::FluidParticle);
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

void IHCEngine::Component::ComputeFluidComponent::createSpacialSSBOs()
{
	// SpacialEntry
	spacialEntrySSBOs.resize(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
	VkDeviceSize bufferSize = sizeof(Graphics::SpacialEntry) * maxParticleCount;
	auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
	uint32_t instanceSize = sizeof(Graphics::SpacialEntry);
	uint32_t instanceCount = maxParticleCount;
	Graphics::IHCBuffer stagingBuffer
	{
		*graphicsManager->GetIHCDevice(),
			instanceSize,
			instanceCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	};
	stagingBuffer.Map();
	stagingBuffer.WriteToBuffer((void*)spacialEntries.data());
	stagingBuffer.Unmap(); 
	for (size_t i = 0; i < Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
	{
		spacialEntrySSBOs[i] = std::make_unique<Graphics::IHCBuffer>(
			*graphicsManager->GetIHCDevice(),
			instanceSize,
			instanceCount,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		graphicsManager->GetIHCDevice()->CopyBuffer(stagingBuffer.GetBuffer(), spacialEntrySSBOs[i]->GetBuffer(), bufferSize);
	}

	// SpacialLookup
	spacialLookupSSBOs.resize(Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
	VkDeviceSize bufferSize2 = sizeof(Graphics::StartIndexForSpacialEntry) * maxParticleCount;
	uint32_t instanceSize2 = sizeof(Graphics::StartIndexForSpacialEntry);
	uint32_t instanceCount2 = maxParticleCount;
	Graphics::IHCBuffer stagingBuffer2
	{
		*graphicsManager->GetIHCDevice(),
			instanceSize2,
			instanceCount2,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	};
	stagingBuffer2.Map();
	stagingBuffer2.WriteToBuffer((void*)spacialLookups.data());
	stagingBuffer2.Unmap();
	for (size_t i = 0; i < Graphics::IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
	{
		spacialLookupSSBOs[i] = std::make_unique<Graphics::IHCBuffer>(
			*graphicsManager->GetIHCDevice(),
			instanceSize2,
			instanceCount2,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		graphicsManager->GetIHCDevice()->CopyBuffer(stagingBuffer2.GetBuffer(), spacialLookupSSBOs[i]->GetBuffer(), bufferSize2);
	}
}

void IHCEngine::Component::ComputeFluidComponent::Attach()
{
	initParticles();
	createVulkanResources();
	Core::GraphicsManagerLocator::GetGraphicsManager()->GetParticleSystem().AddGameObject(this->gameObject, Graphics::PipelineType::COMPUTEFLUID);
}
void IHCEngine::Component::ComputeFluidComponent::Remove()
{
	destroyVulkanResources();
	Core::GraphicsManagerLocator::GetGraphicsManager()->GetParticleSystem().RemoveGameObject(this->gameObject, Graphics::PipelineType::COMPUTEFLUID);
}
