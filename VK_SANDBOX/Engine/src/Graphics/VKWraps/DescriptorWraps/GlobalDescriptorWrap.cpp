#include "../../../pch.h"
#include "GlobalDescriptorWrap.h"

#include "../IHCSwapChain.h"
#include "../IHCBuffer.h"
#include "../VKHelpers.h"

namespace IHCEngine::Graphics
{
	GlobalDescriptorWrap::GlobalDescriptorWrap(IHCDevice& device)
		: IHCDescriptorWrap(device)
	{
	}

	GlobalDescriptorWrap::~GlobalDescriptorWrap()
	{
	}

	void GlobalDescriptorWrap::Setup()
	{
		SetMaxSets(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
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

	void GlobalDescriptorWrap::CreateResources()
	{
		// Allocate memory to store global shader data
		// (camera matrices, global light info)
		// Data that's consistent across a frame
		globalUBOs.resize(IHCSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalUBOs.size(); i++)
		{
			globalUBOs[i] = std::make_unique<IHCBuffer>
				(
					ihcDevice,
					sizeof(GlobalUniformBufferObject),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					//| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
					// we can uncomment this as we're not using minOffsetAlignment for now
				);
			globalUBOs[i]->Map(); // persistent mapping
		}
	}

	void GlobalDescriptorWrap::AllocAndBindDescriptorSets()
	{
		//  binding 0, UNIFORM_BUFFER
		for (int i = 0; i < IHCSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto targetSet = AllocateDescriptorSet();
			auto targetBuffer = globalUBOs[i]->GetDescriptorInfo();
			
			BindBuffer(0, &targetBuffer);
			OverwriteBindings(targetSet);
		}
	}
}
