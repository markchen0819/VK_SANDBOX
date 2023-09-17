#include "../../pch.h"
#include "RenderSystem.h"

#include "../VKWraps/VKHelpers.h"
#include "../VKWraps/IHCDevice.h"
#include "../VKWraps/IHCPipeline.h"
#include "../VKWraps/IHCMesh.h"
#include "../VKWraps/IHCTexture.h"
#include "../../Core/Time/Time.h"
#include "../../Core/Scene/GameObject.h"
#include "../Animation/Model.h"


IHCEngine::Graphics::RenderSystem::RenderSystem(IHCDevice& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
    : ihcDevice{ device }, vkrenderpass{ renderPass }
{
    createDefaultPipelineLayout(descriptorSetLayouts);
    createDefaultPipeline(vkrenderpass);

 //   // example of DefaultPipeline using custom pipeline creation
 //   createCustomPipelineLayout(&defaultGraphicsPipelineLayout, descriptorSetLayouts);
 //   PipelineConfigInfo pipelineConfig{};
 //   IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(pipelineConfig, ihcDevice);
 //   pipelineConfig.renderPass = renderPass;
 //   pipelineConfig.pipelineLayout = defaultGraphicsPipelineLayout;
 //   defaultGraphicsPipeline = createCustomPipeline(pipelineConfig, "Engine/assets/shaders/vert.spv", "Engine/assets/shaders/frag.spv");

    createCustomPipelineLayoutsAndPipelines();
}
IHCEngine::Graphics::RenderSystem::~RenderSystem()
{
    destroyCustomPipelineLayouts();
    destroyDefaultPipelineLayout();
    //  vkDestroyPipeline is called in defaultGraphicsPipeline destructor
}
void IHCEngine::Graphics::RenderSystem::createCustomPipelineLayoutsAndPipelines()
{
    // Wire frame 
    PipelineConfigInfo wireframePipelineConfig{};
    IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(wireframePipelineConfig, ihcDevice);
    wireframePipelineConfig.renderPass = vkrenderpass;
    wireframePipelineConfig.pipelineLayout = defaultGraphicsPipelineLayout; // use default
    wireframePipelineConfig.rasterizer.polygonMode = VK_POLYGON_MODE_LINE; // This is the crucial line for wireframe mode.
    wireframePipelineConfig.rasterizer.lineWidth = 1.0f; // You can adjust this as needed
    wireframePipelineConfig.rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // Or whatever culling mode you prefer
    wireframePipeline = createCustomPipeline(wireframePipelineConfig, "Engine/assets/shaders/vert.spv", "Engine/assets/shaders/frag.spv");

    // Skeletal animation pipeline
    PipelineConfigInfo skeletalAnimationPipelineConfig{};
    IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(skeletalAnimationPipelineConfig, ihcDevice);
    skeletalAnimationPipelineConfig.renderPass = vkrenderpass;
    skeletalAnimationPipelineConfig.pipelineLayout = defaultGraphicsPipelineLayout; // use default
    skeletalAnimationPipeline = createCustomPipeline(skeletalAnimationPipelineConfig, "Engine/assets/shaders/vert.spv", "Engine/assets/shaders/frag.spv");

}
void IHCEngine::Graphics::RenderSystem::destroyCustomPipelineLayouts()
{
    //vkDestroyPipelineLayout(ihcDevice.GetDevice(), PLACEHOLDER, nullptr);
}
#pragma region PipelineLayout (for shaders interface) & Pipeline
void IHCEngine::Graphics::RenderSystem::createDefaultPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
{
    // small data for shader
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0; // not using separate ranges
    pushConstantRange.size = sizeof(SimplePushConstantData);

    // pipeline
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    //pipelineLayoutInfo.setLayoutCount = 0;
    // include globalubo & local textures
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(ihcDevice.GetDevice(),
        &pipelineLayoutInfo, nullptr, &defaultGraphicsPipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}
void IHCEngine::Graphics::RenderSystem::destroyDefaultPipelineLayout()
{
    vkDestroyPipelineLayout(ihcDevice.GetDevice(), defaultGraphicsPipelineLayout, nullptr);
}
void IHCEngine::Graphics::RenderSystem::createDefaultPipeline(VkRenderPass renderPass)
{
    // create pipeline layout & renderpass(swapchain) first before calling
    PipelineConfigInfo pipelineConfig{};
    IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(pipelineConfig, ihcDevice);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = defaultGraphicsPipelineLayout;

    defaultGraphicsPipeline = std::make_unique<IHCEngine::Graphics::IHCPipeline>
        (ihcDevice,
           "Engine/assets/shaders/vert.spv",
           "Engine/assets/shaders/frag.spv",
            pipelineConfig
        );
}
void IHCEngine::Graphics::RenderSystem::createCustomPipelineLayout(VkPipelineLayout* targetPipelineLayout, std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
{
    // small data for shader
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0; // not using separate ranges
    pushConstantRange.size = sizeof(SimplePushConstantData);

    // pipeline
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    //pipelineLayoutInfo.setLayoutCount = 0;
    // include globalubo & local textures
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(ihcDevice.GetDevice(),
        &pipelineLayoutInfo, nullptr, targetPipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}
std::unique_ptr<IHCEngine::Graphics::IHCPipeline> IHCEngine::Graphics::RenderSystem::createCustomPipeline(PipelineConfigInfo& pipelineConfig, std::string vertpath, std::string fragpath)
{
    return std::make_unique<IHCEngine::Graphics::IHCPipeline>
        (ihcDevice,
            vertpath,
            fragpath,
            pipelineConfig
        );
}
#pragma endregion

#pragma region Render (apply pipeline, transform (PushConstants/ UniformBufferObjects), model, then draw)
void IHCEngine::Graphics::RenderSystem::RenderGameObjects(FrameInfo& frameInfo)
{
    if (wireframeEnabled)
    {
        renderWireframePipeline(frameInfo);
    }
    else
    {
        renderDefaultGraphicsPipeline(frameInfo);
        renderSkeletalAnimationPipeline(frameInfo);
    }
}
void IHCEngine::Graphics::RenderSystem::renderDefaultGraphicsPipeline(FrameInfo& frameInfo)
{
    // Bind Pipeline 
    defaultGraphicsPipeline->Bind(frameInfo.commandBuffer);

    // Bind Global Descriptor Set (at set 0)  
    // Common case: Camera Matrices (Proj & View) , Global Lighting Information, Shadow Maps, Environment Maps, IBL
    // Our case:  ubo, sampler
    vkCmdBindDescriptorSets
    (
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        defaultGraphicsPipelineLayout,
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
        IHCEngine::Core::GameObject* gobj = g.second;

        // Has mesh & texture (model in a different pipeline)
        if (gobj->mesh == nullptr) continue;
        if (gobj->texture == nullptr) continue;

        // Bind Local Descriptor Set
        // Common case: Material Textures (Texture, NormalMap, AO), Material Properties, Transform Matrices for Skinned Animations
        // Our case: Texture
        std::string textureID = gobj->texture->GetName();
        auto descriptorSet = frameInfo.textureToDescriptorSetsMap[textureID][frameInfo.frameIndex];
        vkCmdBindDescriptorSets
        (
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            defaultGraphicsPipelineLayout,
            1,
            1,
            &descriptorSet,
            0,
            nullptr
        );

        // Update Local Push Constants (ex: Transform)
        // Common case: Model Matrix, Material Properties, Animation Data:
        // Our case: Model Matrix
        SimplePushConstantData push{};
        push.modelMatrix = gobj->transform.GetModelMatrix();
        push.normalMatrix = glm::mat4(1);
        // potential for lighting
        /*glm::mat4 modelViewMatrix = camera.GetViewMatrix() * transform.GetWorldMatrix();
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));*/
        vkCmdPushConstants
        (
            frameInfo.commandBuffer,
            defaultGraphicsPipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push
        );

        // Bind Mesh(Model)
        gobj->mesh->Bind(frameInfo.commandBuffer);

        // Step 4: Draw Object
        gobj->mesh->Draw(frameInfo.commandBuffer);
    }
}

void IHCEngine::Graphics::RenderSystem::renderWireframePipeline(FrameInfo& frameInfo)
{
    // Bind Pipeline 
    wireframePipeline->Bind(frameInfo.commandBuffer);
    vkCmdBindDescriptorSets
    (
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        defaultGraphicsPipelineLayout,
        0,
        1,
        &frameInfo.globalDescriptorSet,
        0,
        nullptr
    );

    for (auto& g : frameInfo.gameObjects)
    {
        IHCEngine::Core::GameObject* gobj = g.second;

        // temporary
        if (gobj->model != nullptr)
        {
            auto meshes = gobj->model->GetMeshes();
            for (const auto& mesh : meshes)
            {
                MaterialData materialdata = gobj->model->GetMaterialForMesh(mesh.first);

                VkDescriptorSet_T* descriptorSet;
                if (materialdata.diffuseMaps.size() == 0) // no texture, only color
                {
                    descriptorSet = frameInfo.textureToDescriptorSetsMap["plainTexture"][frameInfo.frameIndex];
                }
                else
                {
                    std::string textureID = materialdata.diffuseMaps[0]->GetName();
                    descriptorSet = frameInfo.textureToDescriptorSetsMap[textureID][frameInfo.frameIndex];
                }
                vkCmdBindDescriptorSets
                (
                    frameInfo.commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    defaultGraphicsPipelineLayout,
                    1,
                    1,
                    &descriptorSet,
                    0,
                    nullptr
                );

                SimplePushConstantData push{};
                push.modelMatrix = gobj->transform.GetModelMatrix();
                push.normalMatrix = glm::mat4(1);
                vkCmdPushConstants
                (
                    frameInfo.commandBuffer,
                    defaultGraphicsPipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(SimplePushConstantData),
                    &push
                );
                mesh.second->Bind(frameInfo.commandBuffer);
                mesh.second->Draw(frameInfo.commandBuffer);
            }
            continue;
        }

        if (gobj->mesh == nullptr) continue;
        // Not need to bind texture

        // Model Matrix
        SimplePushConstantData push{};
        push.modelMatrix = gobj->transform.GetModelMatrix();
        push.normalMatrix = glm::mat4(1);
        vkCmdPushConstants
        (
            frameInfo.commandBuffer,
            defaultGraphicsPipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push
        );

        gobj->mesh->Bind(frameInfo.commandBuffer);
        gobj->mesh->Draw(frameInfo.commandBuffer);
    }
}

void IHCEngine::Graphics::RenderSystem::renderSkeletalAnimationPipeline(IHCEngine::Graphics::FrameInfo& frameInfo)
{
    // Bind Pipeline 
    skeletalAnimationPipeline->Bind(frameInfo.commandBuffer);
    // global Descriptor Sets
    vkCmdBindDescriptorSets
    (
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        defaultGraphicsPipelineLayout, // to:do change
        0,
        1,
        &frameInfo.globalDescriptorSet,
        0,
        nullptr
    );

    // For each game object
    for (auto& g : frameInfo.gameObjects)
    {
        IHCEngine::Core::GameObject* gobj = g.second;

        // Has model

        if (gobj->model == nullptr) continue;
        auto meshes = gobj->model->GetMeshes();

        for (const auto& mesh : meshes)
        {
            MaterialData materialdata = gobj->model->GetMaterialForMesh(mesh.first);

            VkDescriptorSet_T* descriptorSet;
            if (materialdata.diffuseMaps.size() == 0) // no texture, only color
            {
                descriptorSet = frameInfo.textureToDescriptorSetsMap["plainTexture"][frameInfo.frameIndex];
            }
            else
            {
                std::string textureID = materialdata.diffuseMaps[0]->GetName();
                descriptorSet = frameInfo.textureToDescriptorSetsMap[textureID][frameInfo.frameIndex];
            }

            // local Descriptor Sets
            vkCmdBindDescriptorSets
            (
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                defaultGraphicsPipelineLayout, // to:do change
                1,
                1,
                &descriptorSet,
                0,
                nullptr
            );

            SimplePushConstantData push{};
            push.modelMatrix = gobj->transform.GetModelMatrix();
            push.normalMatrix = glm::mat4(1);
            vkCmdPushConstants
            (
                frameInfo.commandBuffer,
                defaultGraphicsPipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );
            mesh.second->Bind(frameInfo.commandBuffer);
            mesh.second->Draw(frameInfo.commandBuffer);
        }
    }
}

#pragma endregion
