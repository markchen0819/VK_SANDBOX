#pragma once
#include "Base/CustomPipelineBase.h"

namespace IHCEngine::Component
{
	class ComputeFluidComponent;
}

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

		void createComputePipelineLayout(); 
		void createComputeDensityPressurePipeline();
		void createComputeForcePipeline();
		void createComputeIntegratePipeline();
		void createComputeCopyPipeline();
		void createComputeSpacialHashPipeline();
		void createComputeBitonicMergeSortPipeline();

		VkPipelineLayout graphicsPipelineLayout;
		VkPipeline graphicsPipeline;

		VkPipelineLayout computePipelineLayout; // These compute shaders share the same layout
		VkPipeline computeDensityPressurePipeline;
		VkPipeline computeForcePipeline;
		VkPipeline computeIntegratePipeline;
		VkPipeline computeCopyPipeline; // In case double buffering needed to write back to SSBO out
		VkPipeline computeSpacialHashPipeline;
		VkPipeline computeBitonicMergeSortPipeline;


		// Helpers
		void InsertComputeShaderBarrier(VkCommandBuffer commandBuffer);
		static std::vector<char> readFile(const std::string& filename);
		VkShaderModule createShaderModule(const std::vector<char>& code);

		void GPUSort(int particleCount, FrameInfo& frameInfo, Component::ComputeFluidComponent* fluid);
		void InsertBarrierToUpdateUBO(FrameInfo& frameInfo, Component::ComputeFluidComponent* fluid);
	};
}
