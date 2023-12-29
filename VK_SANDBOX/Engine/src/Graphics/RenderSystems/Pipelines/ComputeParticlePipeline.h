#pragma once
#include "Base/CustomPipelineBase.h"

namespace IHCEngine::Graphics
{
	class ComputeParticlePipeline : public CustomPipelineBase
	{
	public:
		ComputeParticlePipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager);
		~ComputeParticlePipeline() override;

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