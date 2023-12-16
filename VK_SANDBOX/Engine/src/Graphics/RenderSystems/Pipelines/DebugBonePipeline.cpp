#include "../../../pch.h"
#include "DebugBonePipeline.h"

#include "../../VKWraps/VKHelpers.h"
#include "../../VKWraps/IHCDevice.h"
#include "../../VKWraps/IHCPipeline.h"
#include "../../VKWraps/IHCDescriptorManager.h"


#include "../../../Core/Scene/Components/PipelineComponent.h"
#include "../../../Core/Scene/Components/ModelComponent.h"
#include "../../../Core/Scene/Components/AnimatorComponent.h"
#include "../../../Core/Scene/Components/DebugBoneComponent.h"
#include "../../../Core/Scene/Components/IKComponent.h"
#include "../../../Core/Scene/GameObject.h"


namespace IHCEngine::Graphics
{
    DebugBonePipeline::DebugBonePipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager)
        : CustomPipelineBase(device, renderPass, descriptorManager)
    {
        createLayout();
        createPipeline();
    }

    DebugBonePipeline::~DebugBonePipeline()
    {
        destroyPipeline();
    }

    void DebugBonePipeline::Render(FrameInfo& frameInfo)
    {
        // Bind Pipeline 
        pipeline->Bind(frameInfo.commandBuffer);
        // global Descriptor Sets (Camera)
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

        for (auto& gobj : gameObjects)
        {
            if (gobj->IsActive() == false) continue;
            if (!gobj->HasComponent<Component::ModelComponent>()) continue;

            // Has AnimatorComponent or IKComponent
        	// then we can Draw Debug Bones
            auto debugBoneComponent = gobj->GetComponent<Component::DebugBoneComponent>();
            VkDescriptorSet_T* skeletalDescriptorSet;
            auto animatorComponent = gobj->GetComponent<Component::AnimatorComponent>();
            auto ikComponent = gobj->GetComponent<Component::IKComponent>();

            // Has AnimatorComponent
            if (animatorComponent != nullptr && animatorComponent->IsActive() && animatorComponent->HasAnimation())
            {
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

                auto& vertices = animatorComponent->GetDebugBoneVertices();
                debugBoneComponent->UpdateDebugBoneBuffer(vertices, frameInfo);
                debugBoneComponent->DrawDebugBoneBuffer(vertices, frameInfo);
            }
            // Has IKComponent
            else if (ikComponent != nullptr && ikComponent->IsActive()) 
            {
                // Draw Debug Bones
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
                ikComponent->UpdateDebugBoneBuffer(frameInfo);
                ikComponent->DrawDebugBoneBuffer(frameInfo);
            }
            else // No animation
            {

            }
        }
    }

    void DebugBonePipeline::createLayout()
    {
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts
        {
            descriptorManager->GetGlobalDescriptorWrap()->GetDescriptorSetLayout(),
        };

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
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

    void DebugBonePipeline::createPipeline()
    {
        PipelineConfigInfo debugBonePipelineConfig{};
        IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(debugBonePipelineConfig, ihcDevice);
        debugBonePipelineConfig.renderPass = renderPass;
        debugBonePipelineConfig.pipelineLayout = pipelineLayout;
        debugBonePipelineConfig.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        debugBonePipelineConfig.rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
        debugBonePipelineConfig.rasterizer.lineWidth = 1.0f;
        debugBonePipelineConfig.depthStencil.depthTestEnable = VK_FALSE;
        debugBonePipelineConfig.depthStencil.depthWriteEnable = VK_FALSE;

        pipeline = std::make_unique<IHCEngine::Graphics::IHCPipeline>
            (
                ihcDevice,
                "Engine/assets/shaders/debugbonevert.spv",
                "Engine/assets/shaders/debugbonefrag.spv",
                debugBonePipelineConfig
            );
    }

    void DebugBonePipeline::destroyPipeline()
    {
        vkDestroyPipelineLayout(ihcDevice.GetDevice(), pipelineLayout, nullptr);
    }

}
