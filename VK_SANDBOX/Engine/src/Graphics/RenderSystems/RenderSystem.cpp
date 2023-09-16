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
    : ihcDevice{ device }
{
    createPipelineLayout(descriptorSetLayouts);
    createPipeline(renderPass);
    createWireFramePipeline(renderPass);
}
IHCEngine::Graphics::RenderSystem::~RenderSystem()
{
    destroyPipelineLayout();
    //  vkDestroyPipeline is called in ihcPipeline destructor
}

#pragma region PipelineLayout (for shaders interface) & Pipeline
void IHCEngine::Graphics::RenderSystem::createPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
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
void IHCEngine::Graphics::RenderSystem::createWireFramePipeline(VkRenderPass renderPass)
{
    PipelineConfigInfo pipelineConfig{};
    IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(pipelineConfig, ihcDevice);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;

    // wireframe properties
    pipelineConfig.rasterizer.polygonMode = VK_POLYGON_MODE_LINE; // This is the crucial line for wireframe mode.
    pipelineConfig.rasterizer.lineWidth = 1.0f; // You can adjust this as needed
    pipelineConfig.rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // Or whatever culling mode you prefer

    wireframePipeline = std::make_unique<IHCEngine::Graphics::IHCPipeline>
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
        IHCEngine::Core::GameObject* gobj = g.second;

        // Temporary here
        if(gobj->model!=nullptr)
        {
            if (wireframeEnabled)
            {
                wireframePipeline->Bind(frameInfo.commandBuffer);
            }
            else
            {
                ihcPipeline->Bind(frameInfo.commandBuffer);
            }

            auto meshes = gobj->model->GetMeshes();
            for (const auto& mesh: meshes)
            {
                MaterialData materialdata = gobj->model->GetMaterialForMesh(mesh.first);

                VkDescriptorSet_T* descriptorSet;
            	if(materialdata.diffuseMaps.size()==0) // no texture, only color
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
                    pipelineLayout,
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
                    pipelineLayout,
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

        if (gobj->texture == nullptr) continue;

        // Bind its respective Pipeline
        // Each object may have its own pipeline, especially if it uses a different shader 
        // or rendering technique. For instance, some objects might be rendered with a 
        // reflection shader while others use a basic diffuse shader.
        if (wireframeEnabled)
        {
            wireframePipeline->Bind(frameInfo.commandBuffer);
        }
        else
        {
            ihcPipeline->Bind(frameInfo.commandBuffer);
        }
        // Bind Local Descriptor Set
        // Common case: Material Textures (Texture, NormalMap, AO), Material Properties, Transform Matrices for Skinned Animations
        // Our case: None
 
        std::string textureID = gobj->texture->GetName();
        auto descriptorSet =  frameInfo.textureToDescriptorSetsMap[textureID][frameInfo.frameIndex];
        vkCmdBindDescriptorSets
        (
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
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
            pipelineLayout,
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
#pragma endregion
