#include "IHCPipeline.h"

IHCEngine::Graphics::IHCPipeline::IHCPipeline(const VkDevice& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo)
    : device(device)
{
    createGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
}
IHCEngine::Graphics::IHCPipeline::~IHCPipeline()
{
    // Delete shaders as they are linked
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void IHCEngine::Graphics::IHCPipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo)
{

    // Shader Module
    auto vertShaderCode = readFile(vertFilepath);
    auto fragShaderCode = readFile(fragFilepath);

    vertShaderModule = createShaderModule(vertShaderCode);
    fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    // Viewport
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &configInfo.viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &configInfo.scissor;

    // Dynamic state
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // Create Pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    // fixed
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pInputAssemblyState = &configInfo.inputAssembly;
    pipelineInfo.pRasterizationState = &configInfo.rasterizer;
    pipelineInfo.pMultisampleState = &configInfo.multisampling;
    pipelineInfo.pDepthStencilState = &configInfo.depthStencil;
    pipelineInfo.pColorBlendState = &configInfo.colorBlending;
    // dynamic
    pipelineInfo.pDynamicState = &dynamicState;
    // core
    pipelineInfo.layout = configInfo.pipelineLayout;
    pipelineInfo.renderPass = configInfo.renderPass;
    pipelineInfo.subpass = configInfo.subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
}

IHCEngine::Graphics::PipelineConfigInfo IHCEngine::Graphics::IHCPipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height)
{
    PipelineConfigInfo configInfo{};

    // Input assembly
    configInfo.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewport & scissor
    VkViewport viewport{};
    configInfo.viewport.x = 0.0f;
    configInfo.viewport.y = 0.0f;
    configInfo.viewport.width = width;//(float)swapChainExtent.width;
    configInfo.viewport.height = height;// (float)swapChainExtent.height;
    configInfo.viewport.minDepth = 0.0f;
    configInfo.viewport.maxDepth = 1.0f;

    configInfo.scissor.offset = { 0, 0 };
    configInfo.scissor.extent = { width, height };// swapChainExtent;

    // Rasterization
    configInfo.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    configInfo.rasterizer.depthClampEnable = VK_FALSE;
    configInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    configInfo.rasterizer.lineWidth = 1.0f;
    configInfo.rasterizer.cullMode = VK_CULL_MODE_NONE;//VK_CULL_MODE_BACK_BIT;
    //configInfo.rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    configInfo.rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // opengl Y-flip
    configInfo.rasterizer.depthBiasEnable = VK_FALSE;

    // Multismapling
    configInfo.multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    configInfo.multisampling.sampleShadingEnable = VK_TRUE; // enable sample shading in the pipeline //= VK_FALSE;
    configInfo.multisampling.minSampleShading = .2f; // min fraction for sample shading; closer to one is smoother
    configInfo.multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;// msaaSamples; //= VK_SAMPLE_COUNT_1_BIT;

    // Color blending
    configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    configInfo.colorBlendAttachment.blendEnable = VK_FALSE;

    configInfo.colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    configInfo.colorBlending.logicOpEnable = VK_FALSE;
    configInfo.colorBlending.logicOp = VK_LOGIC_OP_COPY;
    configInfo.colorBlending.attachmentCount = 1;
    configInfo.colorBlending.pAttachments = &configInfo.colorBlendAttachment;
    configInfo.colorBlending.blendConstants[0] = 0.0f;
    configInfo.colorBlending.blendConstants[1] = 0.0f;
    configInfo.colorBlending.blendConstants[2] = 0.0f;
    configInfo.colorBlending.blendConstants[3] = 0.0f;

    // Depth and stencil state
    configInfo.depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    configInfo.depthStencil.depthTestEnable = VK_TRUE;
    configInfo.depthStencil.depthWriteEnable = VK_TRUE;
    configInfo.depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    configInfo.depthStencil.depthBoundsTestEnable = VK_FALSE;
    configInfo.depthStencil.stencilTestEnable = VK_FALSE;

    return configInfo;
}

VkShaderModule IHCEngine::Graphics::IHCPipeline::createShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}
std::vector<char> IHCEngine::Graphics::IHCPipeline::readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}