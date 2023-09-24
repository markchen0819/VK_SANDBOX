#pragma once
#include "IHCDescriptorWrap.h"

namespace IHCEngine::Graphics
{
	class IHCBuffer;

	class SkeletalDescriptorWrap : public IHCDescriptorWrap
	{
	public:
		SkeletalDescriptorWrap(IHCDevice& device);
		~SkeletalDescriptorWrap() override;
		void Setup() override;
		void CreateResources() override;
		void AllocAndBindDescriptorSets() override;

		// Get Resources to write into and store
		IHCBuffer* GetAvailableSkeletalUBO();
		void ReleaseSkeletalUBO(IHCBuffer* buffer);
		IHCBuffer* GetDummySkeletalUBO() const { return dummySkeletalUBO.get(); }
		VkDescriptorSet GetDummySkeletalDescriptorSet() {return dummySkeletalDescriptorSet;}



	private:

		const int SKELETAL_COUNT_LIMIT = 20;

		// resource having skeletal bone matrices
		std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> skeletalUBOs;
		std::stack<IHCBuffer*> availableSkeletalUBOs;

		// If animation not used, we still need one for validation errors
		std::unique_ptr<IHCBuffer> dummySkeletalUBO;
		VkDescriptorSet dummySkeletalDescriptorSet;

	};
}
