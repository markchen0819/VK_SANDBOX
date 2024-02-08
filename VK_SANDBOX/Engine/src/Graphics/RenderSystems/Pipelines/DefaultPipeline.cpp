#include "../../../pch.h"
#include "DefaultPipeline.h"

#include "../../VKWraps/VKHelpers.h"
#include "../../VKWraps/IHCDevice.h"
#include "../../VKWraps/IHCPipeline.h"
#include "../../VKWraps/IHCDescriptorManager.h"

#include "../../../Core/Scene/Components/MeshComponent.h"
#include "../../../Core/Scene/Components/TextureComponent.h"
#include "../../../Core/Scene/Components/ComputeParticleComponent.h"
#include "../../../Core/Scene/Components/ComputeGrassComponent.h"
#include "../../../Core/Scene/GameObject.h"


namespace IHCEngine::Graphics
{
    DefaultPipeline::DefaultPipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager)
        : CustomPipelineBase(device, renderPass, descriptorManager)
    {
        createLayout();
        createPipeline();
    }

    DefaultPipeline::~DefaultPipeline()
    {
        destroyPipeline();
    }

    void DefaultPipeline::Render(FrameInfo& frameInfo)
    {
        // Bind Pipeline 
        pipeline->Bind(frameInfo.commandBuffer);
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
            &descriptorManager->GetGlobalDescriptorWrap()
            ->GetDescriptorSets()[frameInfo.frameIndex],
            0,
            nullptr
        );
        // Update Global Push Constants
        // Common case:  Global time value, Viewport Information
        // Our case: None
        for (auto& gobj : gameObjects)
        {

            if (gobj->IsActive() == false) continue;

            // Don't draw mesh binded by the particle system
            if (gobj->HasComponent<Component::ComputeParticleComponent>()) continue;
            if (gobj->HasComponent<Component::ComputeGrassComponent>()) continue;

            if (!gobj->HasComponent<Component::MeshComponent>()) continue;
            if (!gobj->HasComponent<Component::TextureComponent>())
            {
                // Missing Texture Componenent
                assert(true);
                continue;
            }

            // Bind Local Descriptor Set
            // Common case: Material Textures (Texture, NormalMap, AO), Material Properties, Transform Matrices for Skinned Animations
            // Our case: Texture
            auto descriptorSet = gobj->GetComponent<Component::TextureComponent>()->GetDescriptorSets()[frameInfo.frameIndex];
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
            // Bind Mesh and Draw
            gobj->GetComponent<Component::MeshComponent>()->Bind(frameInfo.commandBuffer);
            gobj->GetComponent<Component::MeshComponent>()->Draw(frameInfo.commandBuffer);
        }
    }

    void DefaultPipeline::createLayout()
    {
        // In GLSL, our case
		// globalDescriptorSetLayout set 0, Binding0, UNIFORM_BUFFER
		// textureDescriptorSetLayout set 1, Binding0, COMBINED_IMAGE_SAMPLER
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts
    	{
            descriptorManager->GetGlobalDescriptorWrap()->GetDescriptorSetLayout(),
            descriptorManager ->GetTextureDescriptorWrap()->GetDescriptorSetLayout(),
        };

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0; // not using separate ranges
        pushConstantRange.size = sizeof(SimplePushConstantData);

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

    void DefaultPipeline::createPipeline()
    {
        // create pipeline layout & renderpass(swapchain) first before calling
        PipelineConfigInfo pipelineConfig{};
        IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(pipelineConfig, ihcDevice);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;

        pipeline = std::make_unique<IHCEngine::Graphics::IHCPipeline>
            (ihcDevice,
                "Engine/assets/shaders/vert.spv",
                "Engine/assets/shaders/frag.spv",
                pipelineConfig
            );
    }

    void DefaultPipeline::destroyPipeline()
    {
        vkDestroyPipelineLayout(ihcDevice.GetDevice(), pipelineLayout, nullptr);
    }

}
