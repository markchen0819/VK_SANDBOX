#pragma once
#include "Base/CustomPipelineBase.h"

namespace IHCEngine::Graphics
{
	class ComputeFluidPipeline : public CustomPipelineBase
	{
	public:
		ComputeFluidPipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager);
		~ComputeFluidPipeline() override;

		void Compute(FrameInfo& frameInfo);
		void Render(FrameInfo& frameInfo) override;

	private:
		void createLayout() override;
		void createPipeline() override;
		void destroyPipeline() override;

		void createGraphicsPipelineLayout();
		void createGraphicsPipeline();

		void createComputeForcePipelineLayout();
		void createComputeForcePipeline();

		void createComputeIntegratePipelineLayout();
		void createComputeIntegratePipeline();

		// TO:DO maybe share same layout but different pipeline for compute shaders
		//       add other compute shaders

		VkPipelineLayout graphicsPipelineLayout;
		VkPipeline graphicsPipeline;

		VkPipelineLayout computeDensityPressurePipelineLayout;
		VkPipeline computeDensityPressurePipeline;

		VkPipelineLayout computeForcePipelineLayout;
		VkPipeline computeForcePipeline;

		VkPipelineLayout computeIntegratePipelineLayout;
		VkPipeline computeIntegratePipeline;

		void InsertComputeShaderBarrier(VkCommandBuffer commandBuffer);

		static std::vector<char> readFile(const std::string& filename);
		VkShaderModule createShaderModule(const std::vector<char>& code);
	};
}