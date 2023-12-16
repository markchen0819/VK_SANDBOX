#pragma once
#include "Base/CustomPipelineBase.h"

namespace IHCEngine::Graphics
{
	class SkeletalAnimationPipeline : public CustomPipelineBase
	{
	public:
		SkeletalAnimationPipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager);
		~SkeletalAnimationPipeline() override;

		void Render(FrameInfo& frameInfo) override;

	private:
		void createLayout() override;
		void createPipeline() override;
		void destroyPipeline() override;
	};

}