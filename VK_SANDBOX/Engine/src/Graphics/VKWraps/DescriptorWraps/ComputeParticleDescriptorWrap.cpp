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
			VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_VERTEX_BIT, // need to access for vertex
			1
		);
		CreateLayout();


		CreateResources();
		AllocAndBindDescriptorSets();
	}

	void ComputeParticleDescriptorWrap::CreateResources()
	{
		// Allocate computeParticleUniformBuffers
		// count of PARTICLE_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT

		computeParticleUniformBuffers.resize(PARTICLE_SYSTEM_COUNT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < computeParticleUniformBuffers.size(); i++)
		{
			computeParticleUniformBuffers[i] = std::make_unique<IHCBuffer>
				(
					ihcDevice,
					sizeof(ComputeParticleUniformBufferObject),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
			computeParticleUniformBuffers[i]->Map(); // persistent mapping
		}

	}

	void ComputeParticleDescriptorWrap::AllocAndBindDescriptorSets()
	{
		// Only put into resource stack
		for (const auto& uboUniquePtr : computeParticleUniformBuffers)
		{
			availableComputeParticleUniformBuffers.push(uboUniquePtr.get());
		}
	}

	IHCBuffer* ComputeParticleDescriptorWrap::GetAvailableComputeParticleUBO()
	{
		if (availableComputeParticleUniformBuffers.empty())
		{
			assert("No buffers for allocation. Check if exceed poolsize");
		}
		IHCBuffer* buffer = availableComputeParticleUniformBuffers.top();
		availableComputeParticleUniformBuffers.pop();

		return buffer;
	}

	void ComputeParticleDescriptorWrap::ReleaseComputeParticleUBO(IHCBuffer* buffer)
	{
		vkDeviceWaitIdle(ihcDevice.GetDevice());
		availableComputeParticleUniformBuffers.push(buffer);
	}
}
