#pragma once
#include "../pch.h"
#include "VKHelpers.h"

namespace IHCEngine::Graphics
{
	struct PipelineConfigInfo
	{
		VkViewport viewport{};
		VkRect2D scissor{};
		VkPipelineInputAssemblyStateCreateInfo inputAssembly;
		VkPipelineRasterizationStateCreateInfo rasterizer;
		VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlending;
		VkPipelineDepthStencilStateCreateInfo depthStencil;


		// havent done here and call defaultPipelineConfigInfo
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class IHCPipeline
	{
	public:
		IHCPipeline(
			const VkDevice& device,
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo
		);
		~IHCPipeline();
		// no duplication
		IHCPipeline(const IHCPipeline&) = delete; 
		void operator=(const IHCPipeline&) = delete;

		static  PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
	
	private:

		const VkDevice& device; // retrieve from RenderSystem
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