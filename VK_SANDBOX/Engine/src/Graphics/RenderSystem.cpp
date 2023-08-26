#include "RenderSystem.h"
#include "../Core/Time/Time.h"

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
    
    // Bind Pipeline (if we are only using one pipeline in the rendersystem)
    // ihcPipeline->Bind(frameInfo.commandBuffer);

    // Bind Global Descriptor Set (at set 0)  
    // Common case: Camera Matrices (Proj & View) , Global Lighting Information, Shadow Maps, Environment Maps, IBL
    // Our case:  ubo, sampler
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

    // Update Global Push Constants
    // Common case:  Global time value, Viewport Information
    // Our case: None


    // For each game object
    for (auto& g : frameInfo.gameObjects)
    {
        auto& gobj = g.second;
        if (gobj->model == nullptr) continue;

        // Bind its respective Pipeline
        // Each object may have its own pipeline, especially if it uses a different shader 
        // or rendering technique. For instance, some objects might be rendered with a 
        // reflection shader while others use a basic diffuse shader.
        ihcPipeline->Bind(frameInfo.commandBuffer);

        // Bind Local Descriptor Set
        // Common case: Material Textures (Texture, NormalMap, AO), Material Properties, Transform Matrices for Skinned Animations
        // Our case: None


        // Update Local Push Constants (ex: Transform)
        // Common case: Model Matrix, Material Properties, Animation Data:
        // Our case: Model Matrix
        SimplePushConstantData push{};

        //gobj->transform.SetLocalRotation()
        //push.modelMatrix = gobj->transform.GetLocalModelMatrix();
        push.modelMatrix=
            glm::rotate(glm::mat4(1.0f), IHCEngine::Core::Time::GetInstance().GetElapsedTime() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        push.normalMatrix = glm::mat4(1);
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

        // Bind Mesh(Model)
        gobj->model->Bind(frameInfo.commandBuffer);

        // Step 4: Draw Object
        gobj->model->Draw(frameInfo.commandBuffer);
    }
}
#pragma endregion
