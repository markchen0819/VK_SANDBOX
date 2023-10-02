#pragma once
#include "IHCDescriptorWrap.h"

namespace IHCEngine::Graphics
{
	class IHCBuffer;

	class GlobalDescriptorWrap : public IHCDescriptorWrap
	{
	public:
		GlobalDescriptorWrap(IHCDevice& device);
		~GlobalDescriptorWrap() override;
		void Setup() override;
		void CreateResources() override;
		void AllocAndBindDescriptorSets() override;

		// Get Resources to write into and store
		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>>& GetGlobalUBOs() { return globalUBOs; }
		std::vector<VkDescriptorSet> GetDescriptorSets() { return descriptorSets; }

	private:

		// resource having camera matrices, global light info
		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> globalUBOs;

	};
}
