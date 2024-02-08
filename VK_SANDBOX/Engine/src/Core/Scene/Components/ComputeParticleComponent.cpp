#include "../../../pch.h"
#include "ComputeParticleComponent.h"

#include "../../Locator/GraphicsManagerLocator.h"
#include "../../Locator/SceneManagerLocator.h"
#include "../../../Graphics/VKWraps/IHCSwapChain.h"
#include "../../../Graphics/VKWraps/IHCBuffer.h"
#include "../../../Graphics/VKWraps/IHCDevice.h"
#include "../../../Graphics/Particle/ComputeParticleUniformBufferObject.h"
#include "../../../Graphics/Particle/Particle.h"
#include "../../../Core/Time/Time.h"

#include <random>
#include <omp.h>

IHCEngine::Component::ComputeParticleComponent::ComputeParticleComponent()
	:Component(ComponentType::ComputeParticle)
{

}

void IHCEngine::Component::ComputeParticleComponent::Compute(Graphics::FrameInfo& frameInfo)
{
	float dt = IHCEngine::Core::Time::GetDeltaTime();
	lastFrameTime += dt;

	ubo.deltaTime = dt;
	ubo.accumulatedTime = lastFrameTime;

	ubo.enableAdvection = enableAdvection;
	ubo.flowMaxVelocity = flowMaxVelocity;
	ubo.flowWidth = flowWidth;
	ubo.flowCenter = flowCenter;
	ubo.flowDirection = flowDirection;

	ubo.enableVortex = enableVortex;
	ubo.kappa = kappa;
	ubo.tau = tau;
	ubo.vortexCenter = vortexCenter;
	ubo.vortexAxis = vortexAxis;

	ubo.enableGravity = enableGravity;
	ubo.gravity = gravity;

	ubo.enableBounce = enableBounce;
	ubo.groundHeight = groundHeight;
	ubo.restitution = restitution;

	ubo.enableSpiral = enableSpiral;
	ubo.spiralWithGlobalAxis = spiralWithGlobalAxis;
	ubo.spiralRadius = spiralRadius;
	ubo.sprialAngularSpeed = spiralAngularSpeed;
	ubo.sprialAxisSpeed = spiralAxisSpeed;

	computeParticleUniformBuffers[frameInfo.frameIndex]->WriteToBuffer(&ubo);
	//computeParticleUniformBuffers[frameInfo.frameIndex]->Flush(); // Manual flush, can comment out if using VK_MEMORY_PROPERTY_HOST_COHERENT_BIT

	// Dispatch
	vkCmdDispatch(frameInfo.commandBuffer, particleCount / 256, 1, 1);
}

void IHCEngine::Component::ComputeParticleComponent::Draw(Graphics::FrameInfo& frameInfo)
{
	// Draw Point
	//VkBuffer shaderStorageBuffers[] = { GetSSBO()[frameInfo.frameIndex]->GetBuffer() };
	//VkDeviceSize offsets[] = { 0 };
	//vkCmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, shaderStorageBuffers, offsets);
	//vkCmdDraw(frameInfo.commandBuffer, particleCount, 1, 0, 0);
}

void IHCEngine::Component::ComputeParticleComponent::initParticles()
{
	particles.resize(maxParticleCount);

	// Initialize particles
	std::default_random_engine rndEngine((unsigned)time(nullptr));
	std::uniform_real_distribution<float> colorDistribution(0.0f, 1.0f);
	std::uniform_real_distribution<float> positionDistribution(-5.0f, 5.0f);
	std::uniform_real_distribution<float> lifeTimeDist(1.0f, 10.0f); // Distribution for lifetime

	// auto start = std::chrono::high_resolution_clock::now();
	//#pragma omp parallel for
	// break for loop into possible parallel threads, doesn't speed up after testing, recheck in future
	for (int i = 0; i < particles.size(); ++i)
	{
		auto& particle = particles[i];

		float x = positionDistribution(rndEngine);
		float y = positionDistribution(rndEngine);
		float z = positionDistribution(rndEngine);

		particle.position = glm::vec4 (x, y, z, 0);
		particle.rotation = glm::vec4(1, 0, 0, 0);
		particle.scale = glm::vec4(0.03, 0.03, 0.03, 0);
		particle.velocity = glm::vec4(0, 0, 0, 0);

		float v1 = 0.1f * positionDistribution(rndEngine);
		float v2 = 0.1f * positionDistribution(rndEngine);
		float v3 = 0.1f * positionDistribution(rndEngine);
		particle.velocity = glm::vec4(v1,v2,v3, 0);

		particle.color = glm::vec4(colorDistribution(rndEngine), colorDistribution(rndEngine), 0, 0.5f);

		particle.startPosition = particle.position;
		particle.startRotation = particle.rotation;
		particle.startScale = particle.scale;
		particle.startVelocity = particle.velocity;

		particle.lifeTime = 10.0;
		particle.remainingLifetime = lifeTimeDist(rndEngine); //particle.lifeTime;
	}
	//auto end = std::chrono::high_resolution_clock::now();
	//std::chrono::duration<double, std::milli> duration = end - start;
	//std::cout << "Execution time without parallelization: " << duration.count() << " ms\n";
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
	initParticles();
	createVulkanResources();
	Core::GraphicsManagerLocator::GetGraphicsManager()->GetParticleSystem().AddGameObject(this->gameObject, Graphics::PipelineType::COMPUTEPARTICLE);
}

void IHCEngine::Component::ComputeParticleComponent::Remove()
{
	destroyVulkanResources();
	Core::GraphicsManagerLocator::GetGraphicsManager()->GetParticleSystem().RemoveGameObject(this->gameObject, Graphics::PipelineType::COMPUTEPARTICLE);
}
