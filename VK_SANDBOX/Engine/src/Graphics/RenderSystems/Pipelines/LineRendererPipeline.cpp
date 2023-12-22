#include "../../../pch.h"
#include "LineRendererPipeline.h"

#include "../../VKWraps/VKHelpers.h"
#include "../../VKWraps/IHCDevice.h"
#include "../../VKWraps/IHCPipeline.h"
#include "../../VKWraps/IHCDescriptorManager.h"


#include "../../../Core/Scene/Components/LineRendererComponent.h"
#include "../../../Core/Scene/GameObject.h"

namespace IHCEngine::Graphics
{
	LineRendererPipeline::LineRendererPipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager)
        : CustomPipelineBase(device, renderPass, descriptorManager)
	{
		createLayout();
        createPipeline();
	}

    LineRendererPipeline::~LineRendererPipeline()
	{
        destroyPipeline();
	}

	void LineRendererPipeline::Render(FrameInfo& frameInfo)
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
        // For each game object
        for (auto& gobj: gameObjects)
        {
            if (gobj->IsActive() == false) continue;

            auto lineRendererComponent = gobj->GetComponent<Component::LineRendererComponent>();
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
            lineRendererComponent->Draw(frameInfo);
        }
	}

	void LineRendererPipeline::createLayout()
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

	void LineRendererPipeline::createPipeline()
	{
        PipelineConfigInfo lineRendererPipelineConfig{};
        IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(lineRendererPipelineConfig, ihcDevice);
        lineRendererPipelineConfig.renderPass = renderPass;
        lineRendererPipelineConfig.pipelineLayout = pipelineLayout;
        lineRendererPipelineConfig.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        lineRendererPipelineConfig.rasterizer.lineWidth = 3.0f;

        pipeline = std::make_unique<IHCEngine::Graphics::IHCPipeline>
		(
            ihcDevice,
            "Engine/assets/shaders/linerenderervert.spv",
            "Engine/assets/shaders/linerendererfrag.spv",
            lineRendererPipelineConfig
        );
	}

    void LineRendererPipeline::destroyPipeline()
    {
        vkDestroyPipelineLayout(ihcDevice.GetDevice(), pipelineLayout, nullptr);
    }

}
