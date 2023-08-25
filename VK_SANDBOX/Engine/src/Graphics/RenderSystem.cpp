#include "RenderSystem.h"

IHCEngine::Graphics::RenderSystem::RenderSystem(IHCDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
    : ihcDevice{ device }
{
    createPipelineLayout(globalSetLayout);//
    createPipeline(renderPass);
}
IHCEngine::Graphics::RenderSystem::~RenderSystem()
{
    destroyPipelineLayout();
    //  vkDestroyPipeline is called in ihcPipeline destructor
}

#pragma region PipelineLayout (for shaders interface) & Pipeline
void IHCEngine::Graphics::RenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
{
    // small data for shader
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0; // not using separate ranges
    pushConstantRange.size = sizeof(SimplePushConstantData);

    // bufferobjects for shader (set )
    // can add set1, set2...
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};
    
    // pipeline
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    //pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(ihcDevice.GetDevice(), 
        &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}
void IHCEngine::Graphics::RenderSystem::destroyPipelineLayout()
{
    vkDestroyPipelineLayout(ihcDevice.GetDevice(), pipelineLayout, nullptr);
}
void IHCEngine::Graphics::RenderSystem::createPipeline(VkRenderPass renderPass)
{
    // create pipeline layout & renderpass(swapchain) first before calling

    PipelineConfigInfo pipelineConfig{};
    IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(pipelineConfig, ihcDevice);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;

    ihcPipeline = std::make_unique<IHCEngine::Graphics::IHCPipeline>
        (ihcDevice,
            "Engine/assets/shaders/vert.spv",
            "Engine/assets/shaders/frag.spv",
            pipelineConfig
        );
}
#pragma endregion

#pragma region Render (apply pipeline, transform (PushConstants/ UniformBufferObjects), model, then draw)
void IHCEngine::Graphics::RenderSystem::RenderGameObjects(FrameInfo& frameInfo)
{

    // Step 3: Bind 
    
    // Step 3-1: Bind Pipeline
    ihcPipeline->Bind(frameInfo.commandBuffer);

    // Step 3-2: Bind global descriptor set (at set 0)  (ubo, sampler)
    // (Commonly Proj & View matrix)
    vkCmdBindDescriptorSets
    (
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        &frameInfo.globalDescriptorSet,
        0,
        nullptr
    );

    for (auto& g : frameInfo.gameObjects)
    {
        auto& gobj = g.second;
        if (gobj.model == nullptr) continue;

        // Step 3-3: Update PushConstants (ex: Transform)
        SimplePushConstantData push{};
        //push.normalMatrix = obj.transform.normalMatrix();
        // potential for lighting
        /*glm::mat4 modelViewMatrix = camera.GetViewMatrix() * transform.GetWorldMatrix();
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));*/
        vkCmdPushConstants
        (
            frameInfo.commandBuffer,
            pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push
        );

        // Step 3-4: Bind object material set (at set 1)
        // (Commonly normal, texture ...)
        // 
        // Step 3-5: Bind object descriptor set (at set 2)
        // (Commonly Model matrix)

        // Step 3-6: Bind Model
        gobj.model->Bind(frameInfo.commandBuffer);

        // Step 4: Draw Object
        gobj.model->Draw(frameInfo.commandBuffer);
    }
}
#pragma endregion
