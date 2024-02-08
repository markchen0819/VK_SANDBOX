#include "../../../pch.h"
#include "ComputeGrassDescriptorWrap.h"


#include "../IHCSwapChain.h"
#include "../IHCBuffer.h"
#include "../IHCDevice.h"
#include "../VKHelpers.h"
#include "../../Grass/ComputeGrassUniformBufferObject.h"


namespace IHCEngine::Graphics
{
	ComputeGrassDescriptorWrap::ComputeGrassDescriptorWrap(IHCDevice& device)
		: IHCDescriptorWrap(device)
	{
	}

	ComputeGrassDescriptorWrap::~ComputeGrassDescriptorWrap()
	{
	}

	void ComputeGrassDescriptorWrap::Setup()
	{
		// Sum of sets needed for all particle systems and frames
		SetMaxSets(GRASS_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			GRASS_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			2 * GRASS_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		//AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		//	TEXTURE_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT); //TO:DO textures
		InitializePool();

		// Accessible by the compute stage
		// uniform buffer: feed in grass data
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
			VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT, // need to access for vertex
			1
		);
		CreateLayout();


		CreateResources();
		AllocAndBindDescriptorSets();
	}

	void ComputeGrassDescriptorWrap::CreateResources()
	{
		// Allocate computeGrassUniformBuffers
		// count of GRASS_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT

		computeGrassUniformBuffers.resize(GRASS_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < computeGrassUniformBuffers.size(); i++)
		{
			computeGrassUniformBuffers[i] = std::make_unique<IHCBuffer>
				(
					ihcDevice,
					sizeof(ComputeGrassUniformBufferObject),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
			computeGrassUniformBuffers[i]->Map(); // persistent mapping
		}

	}

	void ComputeGrassDescriptorWrap::AllocAndBindDescriptorSets()
	{
		// Only put into resource stack
		for (const auto& uboUniquePtr : computeGrassUniformBuffers)
		{
			availableComputeGrassUniformBuffers.push(uboUniquePtr.get());
		}
	}

	IHCBuffer* ComputeGrassDescriptorWrap::GetAvailableComputeGrassUBO()
	{
		if (availableComputeGrassUniformBuffers.empty())
		{
			assert("No buffers for allocation. Check if exceed poolsize");
		}
		IHCBuffer* buffer = availableComputeGrassUniformBuffers.top();
		availableComputeGrassUniformBuffers.pop();

		return buffer;
	}

	void ComputeGrassDescriptorWrap::ReleaseComputeGrassUBO(IHCBuffer* buffer)
	{
		vkDeviceWaitIdle(ihcDevice.GetDevice());
		availableComputeGrassUniformBuffers.push(buffer);
	}
}
