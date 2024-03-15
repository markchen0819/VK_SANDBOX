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

#include "../../../Graphics/VKWraps/IHCTexture.h"

IHCEngine::Component::ComputeGrassComponent::ComputeGrassComponent()
	:Component(ComponentType::ComputeGrass)
{

}

void IHCEngine::Component::ComputeGrassComponent::Compute(Graphics::FrameInfo& frameInfo)
{
	updateGrassBladeProperties();
	computeGrassUniformBuffers[frameInfo.frameIndex]->WriteToBuffer(&ubo);
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
	std::uniform_real_distribution<float> scaleOffsetDistribution(-0.8f, 1.2f);
	std::uniform_real_distribution<float> colorDistribution(0.0f, 1.0f);
	std::uniform_real_distribution<float> positionOffsetDistribution(-0.2f, 0.2f); // or voronoi noise
	std::uniform_real_distribution<float> rotationDistribution(0.0f, 360.0f); // angles in degrees

	std::uniform_real_distribution<float> greenVariation(0.2f, 0.4f); // Variation for green
	std::uniform_real_distribution<float> brownVariationR(0.15f, 0.25f); // Variation for brown red
	std::uniform_real_distribution<float> brownVariationG(0.1f, 0.2f); // Variation for brown green

	//std::uniform_real_distribution<float> greenVariation(0.1f, 0.2f); 
	//std::uniform_real_distribution<float> brownVariationR(0.08f, 0.12f);
	//std::uniform_real_distribution<float> brownVariationG(0.04f, 0.07f); 

	const float spacingX = areaSizeX / dimensionX; // Spacing between blades in X
	const float spacingZ = areaSizeZ / dimensionZ; // Spacing between blades in Z

	for (int i = 0; i < dimensionX; ++i)
	{
		for (int j = 0; j < dimensionZ; ++j)
		{
			auto& particle = grassBlades[i * dimensionZ + j];

			// Calculate positions to distribute blades uniformly within 10x10 area
			float x = (j - dimensionX / 2.0f + 0.5f) * spacingX + positionOffsetDistribution(rndEngine); // center the grid, center of cell
			float z = (i - dimensionZ / 2.0f + 0.5f) * spacingZ + positionOffsetDistribution(rndEngine);

			particle.position = glm::vec4(x, 0, z, 0);
			particle.rotation = glm::vec4(1, 0, 0, 0);


			particle.scale = glm::vec4(1.0, 1.0, 1.0, 0);
			particle.scale = glm::vec4(1.0 + scaleOffsetDistribution(rndEngine), 
				1.0 + scaleOffsetDistribution(rndEngine),
				1.0 + scaleOffsetDistribution(rndEngine),
				0);

			//particle.color = glm::vec4(colorDistribution(rndEngine), colorDistribution(rndEngine), 0, 0.5f);

			if (colorDistribution(rndEngine) < 0.5) // Randomly pick a color
			{
				// Set to dark green with some variation
				particle.color = glm::vec4(0.0f, greenVariation(rndEngine), 0.0f, 1.0f);
			}
			else
			{
				// Set to dark brown with some variation
				particle.color = glm::vec4(brownVariationR(rndEngine), brownVariationG(rndEngine), 0.05f, 1.0f);
			}
			// Random rotation
		    // Create a random rotation angle around the y-axis (up)
			//float rotationAngle = glm::radians(0.0);
			float rotationAngle = glm::radians(rotationDistribution(rndEngine));
			// The axis of rotation (up vector)
			glm::vec3 rotationAxis = glm::vec3(0, 1, 0);
			// Creating a quaternion based on the random angle and axis
			glm::quat randomRotation = glm::angleAxis(rotationAngle, rotationAxis);
			// Storing the quaternion in the format (w, x, y, z)
			particle.rotation = glm::vec4(randomRotation.w, randomRotation.x, randomRotation.y, randomRotation.z);

		}
	}
}

void IHCEngine::Component::ComputeGrassComponent::updateGrassBladeProperties()
{
	float dt = IHCEngine::Core::Time::GetDeltaTime();
	lastFrameTime += dt;
	ubo.deltaTime = dt;
	ubo.accumulatedTime = lastFrameTime;

	// Wind
	ubo.windSpeed = grassBladePropertyOverride.windSpeed;
	ubo.windDirection = grassBladePropertyOverride.windDirection;
	ubo.windStrength = grassBladePropertyOverride.windStrength;


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
void IHCEngine::Component::ComputeGrassComponent::SetNoiseTexture(Graphics::IHCTexture* texture)
{
	this->noiseTexture = texture;
}
std::vector<VkDescriptorSet>& IHCEngine::Component::ComputeGrassComponent::GetNoiseTextureDescriptorSet() const
{
	return noiseTexture->GetDescriptorSets();
}

IHCEngine::Graphics::IHCTexture* IHCEngine::Component::ComputeGrassComponent::GetNoiseTexture() const
{
	return noiseTexture;
}

void IHCEngine::Component::ComputeGrassComponent::SetChunkCoords(int chunkX, int chunkY, int gridSizeX, int gridSizeY)
{
	ubo.chunkX = chunkX;
	ubo.chunkY = chunkY;
	ubo.gridSizeX = gridSizeX;
	ubo.gridSizeY = gridSizeY;
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
