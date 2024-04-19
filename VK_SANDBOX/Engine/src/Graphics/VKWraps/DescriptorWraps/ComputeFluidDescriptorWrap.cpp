#include "../../../pch.h"
#include "ComputeFluidDescriptorWrap.h"

#include "../IHCSwapChain.h"
#include "../IHCBuffer.h"
#include "../IHCDevice.h"
#include "../VKHelpers.h"
#include "../../Fluid/ComputeFluidUniformBufferObject.h"

namespace IHCEngine::Graphics
{
	ComputeFluidDescriptorWrap::ComputeFluidDescriptorWrap(IHCDevice& device)
		: IHCDescriptorWrap(device)
	{
	}

	ComputeFluidDescriptorWrap::~ComputeFluidDescriptorWrap()
	{
	}

	void ComputeFluidDescriptorWrap::Setup()
	{
		SetPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
		// Sum of sets needed for all particle systems and frames
		SetMaxSets(FLUID_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			FLUID_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			6 * FLUID_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		//AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		//	TEXTURE_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT); //TO:DO textures
		InitializePool();

		// Accessible by the compute stage
		// uniform buffer: feed in particle data
		// storage buffer: read/write for compute shader


		// ParameterUBO
		AddLayoutBinding(
			0,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_SHADER_STAGE_COMPUTE_BIT,
			1
		);
		// ParticleSSBO In/Out
		AddLayoutBinding(
			1,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT,
			1
		);
		AddLayoutBinding(
			2,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT,
			1
		);
		// Spacial Entry In/Out
		AddLayoutBinding(
			3,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_SHADER_STAGE_COMPUTE_BIT,
			1
		);
		AddLayoutBinding(
			4,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_SHADER_STAGE_COMPUTE_BIT,
			1
		);
		// Spacial Lookup In/Out
		AddLayoutBinding(
			5,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_SHADER_STAGE_COMPUTE_BIT,
			1
		);
		AddLayoutBinding(
			6,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			VK_SHADER_STAGE_COMPUTE_BIT,
			1
		);
		CreateLayout();

		CreateResources();
		AllocAndBindDescriptorSets();
	}

	void ComputeFluidDescriptorWrap::CreateResources()
	{
		// Allocate computeFluidUniformBuffers
		// count of FLUID_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT

		computeFluidUniformBuffers.resize(FLUID_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < computeFluidUniformBuffers.size(); i++)
		{
			computeFluidUniformBuffers[i] = std::make_unique<IHCBuffer>
				(
					ihcDevice,
					sizeof(ComputeFluidUniformBufferObject),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
			computeFluidUniformBuffers[i]->Map(); // persistent mapping
		}

	}

	void ComputeFluidDescriptorWrap::AllocAndBindDescriptorSets()
	{
		// Only put into resource stack
		for (const auto& uboUniquePtr : computeFluidUniformBuffers)
		{
			availableComputeFluidUniformBuffers.push(uboUniquePtr.get());
		}
	}

	void ComputeFluidDescriptorWrap::CustomFreeDescriptorSet(VkDescriptorSet descriptorSet)
	{
		vkDeviceWaitIdle(ihcDevice.GetDevice());
		vkFreeDescriptorSets(ihcDevice.GetDevice(), descriptorPool, 1, &descriptorSet);
	}

	IHCBuffer* ComputeFluidDescriptorWrap::GetAvailableComputeFluidUBO()
	{
		if (availableComputeFluidUniformBuffers.empty())
		{
			assert("No buffers for allocation. Check if exceed poolsize");
		}
		IHCBuffer* buffer = availableComputeFluidUniformBuffers.top();
		availableComputeFluidUniformBuffers.pop();

		return buffer;
	}

	void ComputeFluidDescriptorWrap::ReleaseComputeFluidUBO(IHCBuffer* buffer)
	{
		vkDeviceWaitIdle(ihcDevice.GetDevice());
		availableComputeFluidUniformBuffers.push(buffer);
	}
}
