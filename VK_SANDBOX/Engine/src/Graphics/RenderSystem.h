#pragma once
#include "../pch.h"
#include <stb_image.h>

#include "../Window/AppWindow.h"
#include "VKHelpers.h"
#include "IHCDevice.h"
#include "IHCPipeline.h"
#include "IHCSwapChain.h"
#include "IHCModel.h"
#include "../Core/GameObject.h"
#include "Renderer.h"

const std::string MODEL_PATH = "Engine/assets/models/viking_room/viking_room.obj";
const std::string TEXTURE_PATH = "Engine/assets/models/viking_room/viking_room.png";


namespace IHCEngine::Graphics
{
    class RenderSystem
    {
    public:
        void Init(std::unique_ptr< Window::AppWindow>& w);
        void Update();
        void Shutdown();

        // no duplication
        RenderSystem(const RenderSystem&) = delete;
        RenderSystem &operator=(const RenderSystem&) = delete;

    private:

        ///////////////////
        //// variables ////
        ///////////////////

        Window::AppWindow& appWindow;
        std::unique_ptr<IHCEngine::Graphics::IHCDevice> ihcDevice;
        std::unique_ptr<IHCEngine::Graphics::Renderer> renderer;

        std::unique_ptr<IHCEngine::Graphics::IHCPipeline> ihcPipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<IHCEngine::Graphics::IHCModel> ihcModel;
        std::vector<IHCEngine::Core::GameObject> gameObjects;

        void createPipelineLayout();
        void createPipeline();
        void loadGameObjects();
        void renderGameObjects();

        //VkInstance instance;
        //VkDebugUtilsMessengerEXT debugMessenger;
        //// window
        //VkSurfaceKHR surface;
        //// physical
        //VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        //VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        //// logical
        //VkDevice device;
        //VkQueue graphicsQueue;
        //VkQueue presentQueue;
        // SwapChain
        //VkSwapchainKHR swapChain;
        //std::vector<VkImage> swapChainImages;
        //VkFormat swapChainImageFormat;
        //VkExtent2D swapChainExtent;
        //// ImageView
        //std::vector<VkImageView> swapChainImageViews;
        //// Render pass
        //VkRenderPass renderPass;


        //// Fixed funtions
        //VkPipelineLayout pipelineLayout;
        // graphcis pipeline
        VkDescriptorSetLayout descriptorSetLayout;
        //VkPipeline graphicsPipeline;
        //// Drawing
        //std::vector<VkFramebuffer> swapChainFramebuffers;
        //VkCommandPool commandPool;
        // model
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        // Shader data
        //VkBuffer vertexBuffer;
        //VkDeviceMemory vertexBufferMemory;
        //VkBuffer indexBuffer;
        //VkDeviceMemory indexBufferMemory;
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void*> uniformBuffersMapped;
        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

        //// MSAA
        //VkImage colorImage;
        //VkDeviceMemory colorImageMemory;
        //VkImageView colorImageView;

        //// Depth test
        //VkImage depthImage;
        //VkDeviceMemory depthImageMemory;
        //VkImageView depthImageView;

        // Texture
        uint32_t mipLevels;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;

        // GPU draw, CPU wait frame (var per frame)
        //std::vector<VkCommandBuffer> commandBuffers;
        //std::vector<VkSemaphore> imageAvailableSemaphores;
        //std::vector<VkSemaphore> renderFinishedSemaphores;
        //std::vector<VkFence> inFlightFences;
        //uint32_t currentFrame = 0;
        //bool framebufferResized = false; // get from window resize
        //VkCommandBuffer commandBuffer;
        //VkSemaphore imageAvailableSemaphore;
        //VkSemaphore renderFinishedSemaphore;
        //VkFence inFlightFence;



        ///////////////////
        //// functions ////
        ///////////////////

        void initVulkan();
        void cleanup();

        //// VK instance creation with debugging, extensions ////
        //void createInstance(); 
        //bool checkValidationLayerSupport(); // debugging & callbacks
        //std::vector<const char*>getRequiredExtensions(); // new or platform-specific features
        //static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
        //void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo); //separate debug utils messenger specifically for  vkCreateInstance and vkDestroyInstance calls
        //void setupDebugMessenger();

        //// Create surface to render, pick physical/logical devices ////
        //void createSurface();
        //void pickPhysicalDevice();
        //// pick one that supports rendering & presenting onto the screen (queues)
        //// & supports swapchain
        //bool isDeviceSuitable(VkPhysicalDevice device);
        //QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        //bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        //SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        //VkSampleCountFlagBits getMaxUsableSampleCount(); // for msaa
        //void createLogicalDevice();

        //// Create swapchain and ImageViews for accessing ////
        //void createSwapChain();
        //VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        //VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        //VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        //void createImageViews();
        //VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
        // window size change requires to recreate swap chain
        void recreateSwapChain();
        void cleanupSwapChain();

        //// GraphicsPipeline ////


        //void createGraphicsPipeline()
        //{
        //    auto vertShaderCode = readFile("Engine/assets/shaders/vert.spv");
        //    auto fragShaderCode = readFile("Engine/assets/shaders/frag.spv");

        //    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        //    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        //    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        //    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        //    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        //    vertShaderStageInfo.module = vertShaderModule;
        //    vertShaderStageInfo.pName = "main";

        //    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        //    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        //    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        //    fragShaderStageInfo.module = fragShaderModule;
        //    fragShaderStageInfo.pName = "main";

        //    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        //    // Vertex input
        //    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        //    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        //    auto bindingDescription = Vertex::getBindingDescription();
        //    auto attributeDescriptions = Vertex::getAttributeDescriptions();
        //    vertexInputInfo.vertexBindingDescriptionCount = 1;
        //    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        //    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        //    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        //    // Input assembly
        //    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        //    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        //    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        //    inputAssembly.primitiveRestartEnable = VK_FALSE;
        //    // Viewport
        //    VkPipelineViewportStateCreateInfo viewportState{};
        //    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        //    viewportState.viewportCount = 1;
        //    viewportState.scissorCount = 1;
        //    // Rasterizer
        //    VkPipelineRasterizationStateCreateInfo rasterizer{};
        //    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        //    rasterizer.depthClampEnable = VK_FALSE;
        //    rasterizer.rasterizerDiscardEnable = VK_FALSE;
        //    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        //    rasterizer.lineWidth = 1.0f;
        //    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        //    //rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        //    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // opengl Y-flip
        //    rasterizer.depthBiasEnable = VK_FALSE;
        //    // Multismapling
        //    VkPipelineMultisampleStateCreateInfo multisampling{};
        //    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        //    multisampling.sampleShadingEnable = VK_TRUE; // enable sample shading in the pipeline //= VK_FALSE;
        //    multisampling.minSampleShading = .2f; // min fraction for sample shading; closer to one is smoother
        //    multisampling.rasterizationSamples = msaaSamples; //= VK_SAMPLE_COUNT_1_BIT;
        //    // Depth and stencil state
        //    VkPipelineDepthStencilStateCreateInfo depthStencil{};
        //    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        //    depthStencil.depthTestEnable = VK_TRUE;
        //    depthStencil.depthWriteEnable = VK_TRUE;
        //    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        //    depthStencil.depthBoundsTestEnable = VK_FALSE;
        //    depthStencil.stencilTestEnable = VK_FALSE;

        //    // Color blending
        //    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        //    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        //    colorBlendAttachment.blendEnable = VK_FALSE;

        //    VkPipelineColorBlendStateCreateInfo colorBlending{};
        //    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        //    colorBlending.logicOpEnable = VK_FALSE;
        //    colorBlending.logicOp = VK_LOGIC_OP_COPY;
        //    colorBlending.attachmentCount = 1;
        //    colorBlending.pAttachments = &colorBlendAttachment;
        //    colorBlending.blendConstants[0] = 0.0f;
        //    colorBlending.blendConstants[1] = 0.0f;
        //    colorBlending.blendConstants[2] = 0.0f;
        //    colorBlending.blendConstants[3] = 0.0f;

        //    // Dynamic state
        //    std::vector<VkDynamicState> dynamicStates = {
        //        VK_DYNAMIC_STATE_VIEWPORT,
        //        VK_DYNAMIC_STATE_SCISSOR
        //    };
        //    VkPipelineDynamicStateCreateInfo dynamicState{};
        //    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        //    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        //    dynamicState.pDynamicStates = dynamicStates.data();
        //    // Pipeline layout
        //    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        //    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        //    //pipelineLayoutInfo.setLayoutCount = 0;
        //    //pipelineLayoutInfo.pushConstantRangeCount = 0;
        //    pipelineLayoutInfo.setLayoutCount = 1;
        //    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

        //    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        //        throw std::runtime_error("failed to create pipeline layout!");
        //    }

        //    // Create Pipeline
        //    VkGraphicsPipelineCreateInfo pipelineInfo{};
        //    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        //    pipelineInfo.stageCount = 2;
        //    pipelineInfo.pStages = shaderStages;

        //    pipelineInfo.pVertexInputState = &vertexInputInfo;
        //    pipelineInfo.pInputAssemblyState = &inputAssembly;
        //    pipelineInfo.pViewportState = &viewportState;
        //    pipelineInfo.pRasterizationState = &rasterizer;
        //    pipelineInfo.pMultisampleState = &multisampling;
        //    pipelineInfo.pDepthStencilState = &depthStencil;
        //    pipelineInfo.pColorBlendState = &colorBlending;
        //    pipelineInfo.pDynamicState = &dynamicState;

        //    pipelineInfo.layout = pipelineLayout;
        //    pipelineInfo.renderPass = renderPass;
        //    pipelineInfo.subpass = 0;

        //    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        //    pipelineInfo.basePipelineIndex = -1; // Optional

        //    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        //        throw std::runtime_error("failed to create graphics pipeline!");
        //    }
        //    // Delete shaders as they are linked
        //    vkDestroyShaderModule(device, fragShaderModule, nullptr);
        //    vkDestroyShaderModule(device, vertShaderModule, nullptr);

        //}
        //VkShaderModule createShaderModule(const std::vector<char>& code)
        //{
        //    VkShaderModuleCreateInfo createInfo{};
        //    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        //    createInfo.codeSize = code.size();
        //    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        //    VkShaderModule shaderModule;
        //    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        //        throw std::runtime_error("failed to create shader module!");
        //    }

        //    return shaderModule;
        //}
        //static std::vector<char> readFile(const std::string& filename) {
        //    std::ifstream file(filename, std::ios::ate | std::ios::binary);

        //    if (!file.is_open()) {
        //        throw std::runtime_error("failed to open file!");
        //    }

        //    size_t fileSize = (size_t)file.tellg();
        //    std::vector<char> buffer(fileSize);

        //    file.seekg(0);
        //    file.read(buffer.data(), fileSize);

        //    file.close();

        //    return buffer;
        //}





        //// renderpass (framebuffers)
        //void createRenderPass()
        //{
        //    //   Describe the properties of the color attachment.
        //    VkAttachmentDescription colorAttachment{};
        //    colorAttachment.format = swapChainImageFormat; // Format to match the swap chain images
        //    colorAttachment.samples = msaaSamples; //VK_SAMPLE_COUNT_1_BIT; // No multisampling
        //    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear the values to a constant at the start
        //    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Rendered content will be stored in memory and can be read later
        //    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // We don't care about stencil
        //    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // We don't care about stencil
        //    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // We don't care about initial layout
        //    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        //    //colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Images to be presented in the swap chain
        //    //
        //    VkAttachmentDescription depthAttachment{};
        //    depthAttachment.format = findDepthFormat();
        //    depthAttachment.samples = msaaSamples; //= VK_SAMPLE_COUNT_1_BIT;
        //    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        //    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        //    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        //    // Resolve for MSAA
        //    VkAttachmentDescription colorAttachmentResolve{};
        //    colorAttachmentResolve.format = swapChainImageFormat;
        //    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        //    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        //    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        //    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        //    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        //    // Describe how the color attachment will be used in the subpass.
        //    VkAttachmentReference colorAttachmentRef{};
        //    colorAttachmentRef.attachment = 0; // We have one attachment referenced by index 0
        //    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Images will be used as color attachment

        //    //
        //    VkAttachmentReference depthAttachmentRef{};
        //    depthAttachmentRef.attachment = 1;
        //    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        //    VkAttachmentReference colorAttachmentResolveRef{};
        //    colorAttachmentResolveRef.attachment = 2;
        //    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


        //    // Description of a subpass. A render pass can consist of multiple subpasses.
        //    VkSubpassDescription subpass{};
        //    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // This is a graphics subpass
        //    subpass.colorAttachmentCount = 1; // We have one color attachment
        //    subpass.pColorAttachments = &colorAttachmentRef; // Pointer to the color attachment reference
        //    subpass.pDepthStencilAttachment = &depthAttachmentRef;
        //    subpass.pResolveAttachments = &colorAttachmentResolveRef;

        //    // Declare a structure to hold information about subpass dependencies
        //    VkSubpassDependency dependency{};
        //    // The source subpass, which is the producer of the dependency. 
        //    // VK_SUBPASS_EXTERNAL is a special value that refers to operations happening before or after the render pass.
        //    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        //    // The destination subpass, which is the consumer of the dependency. 
        //    // In this case, it's the first (and possibly only) subpass in the render pass.
        //    dependency.dstSubpass = 0;
        //    // The pipeline stage at which the source (producer) subpass generates the dependency. 
        //    // In this case, it's the stage where color data is written to an attachment (color buffer).
        //    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        //        | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        //    // A bitmask that specifies what types of memory operations at the source must complete before the dependency is satisfied.
        //    // Here it's set to 0, indicating no specific memory operations that need to complete.
        //    dependency.srcAccessMask = 0;
        //    // The pipeline stage at which the destination (consumer) subpass waits for the dependency. 
        //    // Here, it's also the stage where color data is written to an attachment.
        //    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        //        | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        //    // A bitmask that specifies what types of memory operations at the destination will wait for the dependency. 
        //    // Here, it's set to allow write operations to color attachments.
        //    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
        //        | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;



        //    // Create the render pass with the given properties and subpass.
        //    std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
        //    VkRenderPassCreateInfo renderPassInfo{};
        //    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO; // Structure type is for a render pass creation
        //    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        //    renderPassInfo.pAttachments = attachments.data();
        //    renderPassInfo.subpassCount = 1; // We have one subpass
        //    renderPassInfo.pSubpasses = &subpass; // Pointer to the subpass
        //    renderPassInfo.dependencyCount = 1;
        //    renderPassInfo.pDependencies = &dependency;

        //    // Create the render pass. If creation is not successful, throw an error.
        //    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        //        throw std::runtime_error("failed to create render pass!");
        //    }
        //}

        // graphics pipeline (shaders)
        void createDescriptorSetLayout() // uniforms
        {
            // ubo
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = 0;  // The binding point that this binding will be used in the shader
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.pImmutableSamplers = nullptr;
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            // sampler
            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = 1;
            samplerLayoutBinding.descriptorCount = 1;
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            // binding
            std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();

            if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor set layout!");
            }
        }


        // framebuffers
        //void createFramebuffers()
        //{
        //    swapChainFramebuffers.resize(swapChainImageViews.size());

        //    for (size_t i = 0; i < swapChainImageViews.size(); i++)
        //    {
        //        std::array<VkImageView, 3> attachments = {
        //         colorImageView,
        //         depthImageView,
        //         swapChainImageViews[i]
        //        };

        //        VkFramebufferCreateInfo framebufferInfo{};
        //        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        //        framebufferInfo.renderPass = renderPass;
        //        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        //        framebufferInfo.pAttachments = attachments.data();
        //        framebufferInfo.width = swapChainExtent.width;
        //        framebufferInfo.height = swapChainExtent.height;
        //        framebufferInfo.layers = 1;

        //        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
        //            throw std::runtime_error("failed to create framebuffer!");
        //        }
        //    }
        //}

        //// command pool and buffers
        //void createCommandPool()
        //{
        //    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        //    VkCommandPoolCreateInfo poolInfo{};
        //    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        //    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        //    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();



        //    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        //        throw std::runtime_error("failed to create command pool!");
        //    }
        //}

        //void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
        //{
        //    VkCommandBufferBeginInfo beginInfo{};
        //    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        //    // Start recording
        //    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        //        throw std::runtime_error("failed to begin recording command buffer!");
        //    }

        //    // Render pass start 
        //    VkRenderPassBeginInfo renderPassInfo{};
        //    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        //    renderPassInfo.renderPass = renderPass;
        //    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        //    renderPassInfo.renderArea.offset = { 0, 0 };
        //    renderPassInfo.renderArea.extent = swapChainExtent;

        //    std::array<VkClearValue, 2> clearValues{};
        //    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        //    clearValues[1].depthStencil = { 1.0f, 0 };
        //    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        //    renderPassInfo.pClearValues = clearValues.data();

        //    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        //    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        //    // fixed functions chapter, we did specify viewport and scissor state
        //    // for this pipeline to be dynamic.
        //    VkViewport viewport{};
        //    viewport.x = 0.0f;
        //    viewport.y = 0.0f;
        //    viewport.width = (float)swapChainExtent.width;
        //    viewport.height = (float)swapChainExtent.height;
        //    viewport.minDepth = 0.0f;
        //    viewport.maxDepth = 1.0f;
        //    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        //    VkRect2D scissor{};
        //    scissor.offset = { 0, 0 };
        //    scissor.extent = swapChainExtent;
        //    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        //    VkBuffer vertexBuffers[] = { vertexBuffer };
        //    VkDeviceSize offsets[] = { 0 };

        //    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        //    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        //    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
        //    //vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
        //    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
        //    vkCmdEndRenderPass(commandBuffer);

        //    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        //        throw std::runtime_error("failed to record command buffer!");
        //    }
        //    // Render pass ends 
        //}
        // model
        //void loadModel()
        //{
        //    tinyobj::attrib_t attrib; // positions, normals and texture coordinates
        //    std::vector<tinyobj::shape_t> shapes; //separate objects and their faces
        //    std::vector<tinyobj::material_t> materials;
        //    std::string warn, err;

        //    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
        //        throw std::runtime_error(warn + err);
        //    }

        //    // combine all of the faces in the file into a single model,
        //    // triangulation feature has already made sure that there are three vertices per face
        //    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        //    for (const auto& shape : shapes)
        //    {
        //        for (const auto& index : shape.mesh.indices)
        //        {
        //            Vertex vertex{};

        //            vertex.pos =
        //            {
        //                attrib.vertices[3 * index.vertex_index + 0],
        //                attrib.vertices[3 * index.vertex_index + 1],
        //                attrib.vertices[3 * index.vertex_index + 2]
        //            };

        //            vertex.texCoord =
        //            {
        //                attrib.texcoords[2 * index.texcoord_index + 0],
        //                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
        //            };

        //            vertex.color = { 1.0f, 1.0f, 1.0f };

        //            if (uniqueVertices.count(vertex) == 0) {
        //                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        //                vertices.push_back(vertex);
        //            }

        //            indices.push_back(uniqueVertices[vertex]);
        //        }
        //    }
        //}

        // shader buffers
        //void createVertexBuffer()
        //{
        //    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        //    // temporary buffer accessed by CPU and GPU
        //    VkBuffer stagingBuffer;
        //    VkDeviceMemory stagingBufferMemory;
        //    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        //    // map temporary buffer memory to CPU address space, Copy vertex data to staging buffer
        //    void* data;
        //    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        //    memcpy(data, vertices.data(), (size_t)bufferSize);
        //    vkUnmapMemory(device, stagingBufferMemory);

        //    // actual vertex buffer, staging buffer to the vertex buffer by the GPU
        //    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
        //    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        //    vkDestroyBuffer(device, stagingBuffer, nullptr);
        //    vkFreeMemory(device, stagingBufferMemory, nullptr);
        //}
        void createIndexBuffer()
        {
            VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void* data;
            vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, indices.data(), (size_t)bufferSize);
            vkUnmapMemory(device, stagingBufferMemory);

            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

            copyBuffer(stagingBuffer, indexBuffer, bufferSize);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
        }
        void createUniformBuffers()
        {
            VkDeviceSize bufferSize = sizeof(UniformBufferObject);

            uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
            uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
            uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

                vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
            }
        }
        void createDescriptorPool()
        {
            std::array<VkDescriptorPoolSize, 2> poolSizes{};
            poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
            poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            poolInfo.pPoolSizes = poolSizes.data();
            poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

            if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor pool!");
            }
        }
        void createDescriptorSets()
        {
            // create one descriptor set for each frame in flight
            std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = descriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
            allocInfo.pSetLayouts = layouts.data();

            descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
            if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }

            // configure each descriptor set.
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                VkDescriptorBufferInfo bufferInfo{}; // which buffer the descriptor will refer to
                bufferInfo.buffer = uniformBuffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(UniformBufferObject);

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = textureImageView;
                imageInfo.sampler = textureSampler;

                std::array<VkWriteDescriptorSet, 2> descriptorWrites{}; // specifies how to update a descriptor set
                descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[0].dstSet = descriptorSets[i];
                descriptorWrites[0].dstBinding = 0;
                descriptorWrites[0].dstArrayElement = 0;
                descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[0].descriptorCount = 1;
                descriptorWrites[0].pBufferInfo = &bufferInfo;

                descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[1].dstSet = descriptorSets[i];
                descriptorWrites[1].dstBinding = 1;
                descriptorWrites[1].dstArrayElement = 0;
                descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[1].descriptorCount = 1;
                descriptorWrites[1].pImageInfo = &imageInfo;

                vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
            }
        }

        //void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
        //{
        //    // create buffer
        //    VkBufferCreateInfo bufferInfo{};
        //    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        //    //bufferInfo.size = sizeof(vertices[0]) * vertices.size();
        //    //bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        //    bufferInfo.size = size;
        //    bufferInfo.usage = usage; // now we can do different types of buffer
        //    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        //    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        //        throw std::runtime_error("failed to create buffer!");
        //    }
        //    // allocate memory

        //    VkMemoryRequirements memRequirements;
        //    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        //    VkMemoryAllocateInfo allocInfo{};
        //    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        //    allocInfo.allocationSize = memRequirements.size;
        //    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        //    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        //        throw std::runtime_error("failed to allocate buffer memory!");
        //    }

        //    // bind
        //    vkBindBufferMemory(device, buffer, bufferMemory, 0);
        //}
        //void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
        //{
        //    // use temp command buffer to do Memory transfer operations 
        //    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        //    VkBufferCopy copyRegion{};
        //    copyRegion.size = size;
        //    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        //    endSingleTimeCommands(commandBuffer);

        //}
        //uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
        //{
        //    VkPhysicalDeviceMemoryProperties memProperties;
        //    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        //    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        //        if ((typeFilter & (1 << i)) &&
        //            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
        //            return i;
        //        }
        //    }

        //    throw std::runtime_error("failed to find suitable memory type!");
        //}

        //// helper on buffers
        //VkCommandBuffer beginSingleTimeCommands()
        //{
        //    VkCommandBufferAllocateInfo allocInfo{};
        //    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        //    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        //    allocInfo.commandPool = commandPool;
        //    allocInfo.commandBufferCount = 1;
        //    VkCommandBuffer commandBuffer;
        //    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        //    // record the command
        //    VkCommandBufferBeginInfo beginInfo{};
        //    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        //    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        //    vkBeginCommandBuffer(commandBuffer, &beginInfo);

        //    return commandBuffer;
        //}
        //void endSingleTimeCommands(VkCommandBuffer commandBuffer)
        //{
        //    vkEndCommandBuffer(commandBuffer);

        //    // execute command
        //    VkSubmitInfo submitInfo{};
        //    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        //    submitInfo.commandBufferCount = 1;
        //    submitInfo.pCommandBuffers = &commandBuffer;
        //    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        //    vkQueueWaitIdle(graphicsQueue);

        //    // free
        //    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
        //}

        ////MSAA
        //void createColorResources()
        //{
        //    VkFormat colorFormat = swapChainImageFormat;

        //    createImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory);
        //    colorImageView = createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
        //}
        //// depth test
        //void createDepthResources()
        //{
        //    VkFormat depthFormat = findDepthFormat();
        //    createImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
        //    depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
        //}
        //VkFormat findDepthFormat()
        //{
        //    return findSupportedFormat(
        //        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        //        VK_IMAGE_TILING_OPTIMAL,
        //        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        //    );
        //}
        bool hasStencilComponent(VkFormat format)
        {
            return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
        }
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
        {
            for (VkFormat format : candidates)
            {
                VkFormatProperties props;
                vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

                if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                    return format;
                }
                else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                    return format;
                }
            }
            throw std::runtime_error("failed to find supported format!");
        }

        // load textures
        void createTextureImage()
        {
            int texWidth, texHeight, texChannels;
            //stbi_uc* pixels = stbi_load("Engine/assets/textures/statue-1275469_1280.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

            VkDeviceSize imageSize = texWidth * texHeight * 4;
            mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

            if (!pixels) {
                throw std::runtime_error("failed to load texture image!");
            }

            // buffer
            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

            void* data;
            vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
            vkUnmapMemory(device, stagingBufferMemory);

            stbi_image_free(pixels);

            // image
            createImage(texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT,
                VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
                , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                textureImage, textureImageMemory);

            // copy buffer to image
            transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
            copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
            //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
            //transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

            // clean up
            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);


            generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);
        }

        void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
        {
            // Check if image format supports linear blitting
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

            if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
                throw std::runtime_error("texture image format does not support linear blitting!");
            }

            VkCommandBuffer commandBuffer = beginSingleTimeCommands();

            // Create barrier
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = image;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.subresourceRange.levelCount = 1;

            int32_t mipWidth = texWidth;
            int32_t mipHeight = texHeight;

            for (uint32_t i = 1; i < mipLevels; i++)
            {
                // Prepare Blit operation
                barrier.subresourceRange.baseMipLevel = i - 1;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                    0, nullptr,
                    0, nullptr,
                    1, &barrier);

                // Prepare Blit operation
                VkImageBlit blit{};
                blit.srcOffsets[0] = { 0, 0, 0 };
                blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = 1;
                blit.dstOffsets[0] = { 0, 0, 0 };
                blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = i;
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount = 1;

                // Perform Blit operation
                vkCmdBlitImage(commandBuffer,
                    image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1, &blit,
                    VK_FILTER_LINEAR);

                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                    0, nullptr,
                    0, nullptr,
                    1, &barrier);

                if (mipWidth > 1) mipWidth /= 2;
                if (mipHeight > 1) mipHeight /= 2;
            }
            // Last mipmap level need explicit one to transfer layout (cause no blit)
            barrier.subresourceRange.baseMipLevel = mipLevels - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            endSingleTimeCommands(commandBuffer);
        }

        //void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
        //    VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
        //    VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
        //{
        //    VkImageCreateInfo imageInfo{};
        //    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        //    imageInfo.imageType = VK_IMAGE_TYPE_2D;
        //    imageInfo.extent.width = width;
        //    imageInfo.extent.height = height;
        //    imageInfo.extent.depth = 1;
        //    imageInfo.mipLevels = mipLevels;
        //    imageInfo.arrayLayers = 1;
        //    imageInfo.format = format;
        //    imageInfo.tiling = tiling;
        //    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //    imageInfo.usage = usage;
        //    imageInfo.samples = numSamples;
        //    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        //    if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        //        throw std::runtime_error("failed to create image!");
        //    }

        //    VkMemoryRequirements memRequirements;
        //    vkGetImageMemoryRequirements(device, image, &memRequirements);

        //    VkMemoryAllocateInfo allocInfo{};
        //    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        //    allocInfo.allocationSize = memRequirements.size;
        //    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        //    if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        //        throw std::runtime_error("failed to allocate image memory!");
        //    }

        //    vkBindImageMemory(device, image, imageMemory, 0);
        //}
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
        {
            VkCommandBuffer commandBuffer = beginSingleTimeCommands();

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = mipLevels;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            VkPipelineStageFlags sourceStage;
            VkPipelineStageFlags destinationStage;

            if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }
            else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            }
            else {
                throw std::invalid_argument("unsupported layout transition!");
            }

            vkCmdPipelineBarrier(
                commandBuffer,
                sourceStage, destinationStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );

            endSingleTimeCommands(commandBuffer);
        }
        //void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
        //{
        //    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        //    VkBufferImageCopy region{};
        //    region.bufferOffset = 0;
        //    region.bufferRowLength = 0;
        //    region.bufferImageHeight = 0;
        //    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        //    region.imageSubresource.mipLevel = 0;
        //    region.imageSubresource.baseArrayLayer = 0;
        //    region.imageSubresource.layerCount = 1;
        //    region.imageOffset = { 0, 0, 0 };
        //    region.imageExtent = {
        //        width,
        //        height,
        //        1
        //    };

        //    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        //    endSingleTimeCommands(commandBuffer);
        //}

        void createTextureImageView()
        {
            textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
        }
        void createTextureSampler()
        {
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(physicalDevice, &properties);

            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
            //samplerInfo.anisotropyEnable = VK_FALSE;
            //samplerInfo.maxAnisotropy = 1.0f;
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.minLod = 0.0f;
            //samplerInfo.minLod = static_cast<float>(mipLevels / 2); // testing mipmap
            samplerInfo.maxLod = static_cast<float>(mipLevels);
            samplerInfo.mipLodBias = 0.0f;

            if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture sampler!");
            }

        }
        // CPU frame waiting, GPU drawing
        //void createSyncObjects()
        //{
        //    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        //    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        //    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        //    VkSemaphoreCreateInfo semaphoreInfo{};
        //    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        //    VkFenceCreateInfo fenceInfo{};
        //    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        //    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // make sure the first frame can signal the fence without previous frame

        //    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        //    {
        //        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
        //            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
        //            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
        //        {
        //            throw std::runtime_error("failed to create synchronization objects for a frame!");
        //        }
        //    }
        //}

        // final draw on screen
        //void drawFrame()
        //{
        //    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        //    uint32_t imageIndex;
        //    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
        //    if (result == VK_ERROR_OUT_OF_DATE_KHR)
        //    {
        //        recreateSwapChain();
        //        return;
        //    }
        //    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        //    {
        //        throw std::runtime_error("failed to acquire swap chain image!");
        //    }
        //    updateUniformBuffer(currentFrame);
        //    // Only reset the fence if we are submitting work
        //    vkResetFences(device, 1, &inFlightFences[currentFrame]);

        //    vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        //    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);
        //    VkSubmitInfo submitInfo{};
        //    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        //    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        //    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        //    submitInfo.waitSemaphoreCount = 1;
        //    submitInfo.pWaitSemaphores = waitSemaphores;
        //    submitInfo.pWaitDstStageMask = waitStages;
        //    submitInfo.commandBufferCount = 1;
        //    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

        //    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
        //    submitInfo.signalSemaphoreCount = 1;
        //    submitInfo.pSignalSemaphores = signalSemaphores;

        //    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        //        throw std::runtime_error("failed to submit draw command buffer!");
        //    }

        //    VkPresentInfoKHR presentInfo{};
        //    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        //    presentInfo.waitSemaphoreCount = 1;
        //    presentInfo.pWaitSemaphores = signalSemaphores;

        //    VkSwapchainKHR swapChains[] = { swapChain };
        //    presentInfo.swapchainCount = 1;
        //    presentInfo.pSwapchains = swapChains;
        //    presentInfo.pImageIndices = &imageIndex;
        //    presentInfo.pResults = nullptr; // Optional

        //    result = vkQueuePresentKHR(presentQueue, &presentInfo);


        //    bool framebufferResized = appWindow->IsWindowResized();

        //    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
        //    {
        //        framebufferResized = false;
        //        recreateSwapChain();
        //    }
        //    else if (result != VK_SUCCESS)
        //    {
        //        throw std::runtime_error("failed to present swap chain image!");
        //    }


        //    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        //}
        //void updateUniformBuffer(uint32_t currentImage)
        //{
        //    static auto startTime = std::chrono::high_resolution_clock::now();
        //    auto currentTime = std::chrono::high_resolution_clock::now();
        //    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        //    UniformBufferObject ubo{};
        //    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        //    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        //    ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
        //    ubo.proj[1][1] *= -1;

        //    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

        //}

    };

}