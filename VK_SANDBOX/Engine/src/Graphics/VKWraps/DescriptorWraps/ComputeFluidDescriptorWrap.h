#pragma once
#include "IHCDescriptorWrap.h"

namespace IHCEngine::Graphics
{
	class IHCBuffer;

	class ComputeFluidDescriptorWrap : public IHCDescriptorWrap
	{
	public:
		ComputeFluidDescriptorWrap(IHCDevice& device);
		~ComputeFluidDescriptorWrap() override;
		void Setup() override;
		void CreateResources() override;
		void AllocAndBindDescriptorSets() override;
		void CustomFreeDescriptorSet(VkDescriptorSet descriptorSet);
		// Get Resources to write into and store
		IHCBuffer* GetAvailableComputeFluidUBO();
		void ReleaseComputeFluidUBO(IHCBuffer* buffer);

	private:
		const int FLUID_SYSTEM_COUNT = 10;
		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> computeFluidUniformBuffers;
		std::stack<IHCBuffer*> availableComputeFluidUniformBuffers;

	};
}
