#include "../../../pch.h"
#include "SkeletalDescriptorWrap.h"

#include "../IHCSwapChain.h"
#include "../IHCBuffer.h"
#include "../IHCDevice.h"
#include "../VKHelpers.h"

namespace IHCEngine::Graphics
{
	SkeletalDescriptorWrap::SkeletalDescriptorWrap(IHCDevice& device)
		: IHCDescriptorWrap(device)
	{
	}

	SkeletalDescriptorWrap::~SkeletalDescriptorWrap()
	{
	}

	void SkeletalDescriptorWrap::Setup()
	{
		SetMaxSets(SKELETAL_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 
			SKELETAL_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		InitializePool();

		AddLayoutBinding(
			0, 
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_SHADER_STAGE_VERTEX_BIT
		);
		CreateLayout();

		CreateResources();
		AllocAndBindDescriptorSets();
	}

	void SkeletalDescriptorWrap::CreateResources()
	{
		// Skeletal
		skeletalUBOs.resize(SKELETAL_COUNT_LIMIT * IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < skeletalUBOs.size(); i++)
		{
			skeletalUBOs[i] = std::make_unique<IHCBuffer>
				(
					ihcDevice,
					sizeof(SkeletalUniformBufferObject),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					//| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
					// we can uncomment this as we're not using minOffsetAlignment for now
				);
			skeletalUBOs[i]->Map(); // persistent mapping
		}
		// Dummy
		dummySkeletalUBO = std::make_unique<IHCBuffer>
			(
				ihcDevice,
				sizeof(SkeletalUniformBufferObject),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				//| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
				// we can uncomment this as we're not using minOffsetAlignment for now
			);
		dummySkeletalUBO->Map();
	}

	void SkeletalDescriptorWrap::AllocAndBindDescriptorSets()
	{
		for (const auto& uboUniquePtr : skeletalUBOs)
		{
			availableSkeletalUBOs.push(uboUniquePtr.get());
		}

		dummySkeletalDescriptorSet = AllocateDescriptorSet();
		auto bufferInfo = dummySkeletalUBO->GetDescriptorInfo();
		BindBuffer(0, &bufferInfo);
		OverwriteBindings(dummySkeletalDescriptorSet);
	}

	IHCBuffer* SkeletalDescriptorWrap::GetAvailableSkeletalUBO()
	{
		if (availableSkeletalUBOs.empty())
		{
			assert("No buffers for allocation. Check if exceed poolsize");
		}
		IHCBuffer* buffer = availableSkeletalUBOs.top();
		availableSkeletalUBOs.pop();

		return buffer;
	}

	void SkeletalDescriptorWrap::ReleaseSkeletalUBO(IHCBuffer* buffer)
	{
		vkDeviceWaitIdle(ihcDevice.GetDevice());
		availableSkeletalUBOs.push(buffer);
	}
}
