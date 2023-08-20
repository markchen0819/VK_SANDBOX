#pragma once
#include "../pch.h"
#include "VKHelpers.h"
#include "IHCDevice.h"

namespace IHCEngine::Graphics
{
	struct PipelineConfigInfo
	{
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		// fixed
		VkPipelineInputAssemblyStateCreateInfo inputAssembly;
		VkPipelineRasterizationStateCreateInfo rasterizer;
		VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlending;
		VkPipelineDepthStencilStateCreateInfo depthStencil;

		// dynamic
		//VkViewport viewport{}; not needed as we use dynamic
		//VkRect2D scissor{};
		VkPipelineViewportStateCreateInfo viewportInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;

		// shader
		//std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		//std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		// core
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class IHCPipeline
	{
	public:
		IHCPipeline(
			IHCDevice& device,
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo
		);
		~IHCPipeline();
		// no duplication
		IHCPipeline(const IHCPipeline&) = delete;
		IHCPipeline &operator=(const IHCPipeline&) = delete;

		static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo, IHCDevice& device);
		void Bind(VkCommandBuffer commandBuffer);

	private:

		IHCDevice& device; 
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;


		void createGraphicsPipeline(
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo
		);
		// shaders
		static std::vector<char> readFile(const std::string& filename);
		VkShaderModule createShaderModule(const std::vector<char>& code);

	};

}