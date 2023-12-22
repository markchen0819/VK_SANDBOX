#pragma once
#include "Base/CustomPipelineBase.h"

namespace IHCEngine::Graphics
{
	class WireframePipeline : public CustomPipelineBase
	{
	public:
		WireframePipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager);
		~WireframePipeline() override;

		void Render(FrameInfo& frameInfo) override;

	private:
		void createLayout() override;
		void createPipeline() override;
		void destroyPipeline() override;
	};

}