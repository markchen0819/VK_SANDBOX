#pragma once
#include "IHCDescriptorWrap.h"

namespace IHCEngine::Graphics
{
	class IHCBuffer;

	class ComputeGrassDescriptorWrap : public IHCDescriptorWrap
	{
	public:
		ComputeGrassDescriptorWrap(IHCDevice& device);
		~ComputeGrassDescriptorWrap() override;
		void Setup() override;
		void CreateResources() override;
		void AllocAndBindDescriptorSets() override;
		void CustomFreeDescriptorSet(VkDescriptorSet descriptorSet);

		// Get Resources to write into and store
		IHCBuffer* GetAvailableComputeGrassUBO();
		void ReleaseComputeGrassUBO(IHCBuffer* buffer);

	private:

		const int GRASS_SYSTEM_COUNT = 30;
		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> computeGrassUniformBuffers;
		std::stack<IHCBuffer*> availableComputeGrassUniformBuffers;
	};
}
