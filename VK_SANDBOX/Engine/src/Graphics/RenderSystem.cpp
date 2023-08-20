#include "RenderSystem.h"

IHCEngine::Graphics::RenderSystem::RenderSystem(IHCDevice& device, VkRenderPass renderPass)
    : ihcDevice{ device }
{
    createPipelineLayout();//globalSetLayout
    createPipeline(renderPass);
}
IHCEngine::Graphics::RenderSystem::~RenderSystem()
{
    destroyPipelineLayout();
    //  vkDestroyPipeline is called in ihcPipeline destructor
}

#pragma region PipelineLayout (for shaders interface) & Pipeline
void IHCEngine::Graphics::RenderSystem::createPipelineLayout()
{
    // small data for shader
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0; // not using separate ranges
    pushConstantRange.size = sizeof(SimplePushConstantData);

    // bufferobjects for shader
     
    
    // pipeline
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    //pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
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
void IHCEngine::Graphics::RenderSystem::RenderGameObjects()
{

    // Step 3: Bind model, PushConstants, UniformBufferObjects
    // Step 3-1: Bind Pipeline
    ihcPipeline->Bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        &frameInfo.globalDescriptorSet,
        0,
        nullptr);

    for (auto& kv : frameInfo.gameObjects)
    {
        // Step 3-1: Bind Transform
        auto& obj = kv.second;
        if (obj.model == nullptr) continue;
        SimplePushConstantData push{};
        push.modelMatrix = obj.transform.mat4();
        push.normalMatrix = obj.transform.normalMatrix();

        // Step 3-1: Bind PushConstants
        vkCmdPushConstants(
            frameInfo.commandBuffer,
            pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push);

        // Step 3-1: Bind Model
        obj.model->bind(frameInfo.commandBuffer);

        // Step 4: Draw
        obj.model->draw(frameInfo.commandBuffer);
    }
}
#pragma endregion










void IHCEngine::Graphics::RenderSystem::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{


    ///////////////////////////////
    ////// Render pass start //////
    ///////////////////////////////
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    // Step 1: Bind pipeline
    ihcPipeline->Bind(commandBuffer);


    // Step 2: Bind model (setup vertices, indices, normal, uv)
    ihcModel->Bind(commandBuffer);

    // Step 3: PushConstant
    //vkCmdPushConstants(
    //    frameInfo.commandBuffer,
    //    pipelineLayout,
    //    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
    //    0,
    //    sizeof(SimplePushConstantData),
    //    &push);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
    
    // Step 3: Draw
    ihcModel->Draw(commandBuffer);

    vkCmdEndRenderPass(commandBuffer);
    /////////////////////////////
    ////// Render pass end //////
    /////////////////////////////
    // End recording
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}



void IHCEngine::Graphics::RenderSystem::loadGameObjects()
{
    auto testGobj = IHCEngine::Core::GameObject::CreateGameObject();
    //testGobj.AddComponent<X>();
    gameObjects.push_back(testGobj);

}

