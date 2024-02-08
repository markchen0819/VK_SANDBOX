#pragma once
#include "Base/CustomPipelineBase.h"

// Same as ComputeParticlePipeline
// Contains
// one for compute transforms
// one for render

namespace IHCEngine::Graphics
{
	class ComputeGrassPipeline : public CustomPipelineBase
	{
	public:
		ComputeGrassPipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager);
		~ComputeGrassPipeline() override;

		void Compute(FrameInfo& frameInfo);
		void Render(FrameInfo& frameInfo) override;

	private:
		void createLayout() override;
		void createPipeline() override;
		void destroyPipeline() override;

		void createGraphicsPipelineLayout();
		void createComputePipelineLayout();
		void createGraphicsPipeline();
		void createComputePipeline();

		VkPipelineLayout graphicsPipelineLayout;
		VkPipeline graphicsPipeline;

		VkPipelineLayout computePipelineLayout;
		VkPipeline computePipeline;

		static std::vector<char> readFile(const std::string& filename);
		VkShaderModule createShaderModule(const std::vector<char>& code);
	};
}