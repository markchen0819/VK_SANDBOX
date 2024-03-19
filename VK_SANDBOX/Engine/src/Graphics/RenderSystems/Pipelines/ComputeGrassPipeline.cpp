#include "../../../pch.h"
#include "ComputeGrassPipeline.h"

#include "../../VKWraps/VKHelpers.h"
#include "../../VKWraps/IHCDevice.h"
#include "../../VKWraps/IHCBuffer.h"
#include "../../VKWraps/IHCDescriptorManager.h"
#include "../../../Core/Scene/Components/ComputeGrassComponent.h"
#include "../../../Core/Scene/Components/MeshComponent.h"
#include "../../../Core/Scene/Components/TextureComponent.h"
#include "../../../Core/Scene/GameObject.h"

namespace IHCEngine::Graphics
{
    ComputeGrassPipeline::ComputeGrassPipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager)
        : CustomPipelineBase(device, renderPass, descriptorManager)
    {
        createLayout();
        createPipeline();
    }

    ComputeGrassPipeline::~ComputeGrassPipeline()
    {
        destroyPipeline();
    }

    void ComputeGrassPipeline::Compute(FrameInfo& frameInfo)
    {
        // Bind Pipeline
        vkCmdBindPipeline(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);

        for (auto& gobj : gameObjects)
        {
            if (gobj->IsActive() == false) continue;
            auto component = gobj->GetComponent<Component::ComputeGrassComponent>();
            if (component->IsActive() == false) continue;

            // 1 ubo 2 ssbo
            auto descriptorSet = component->GetDescriptorSets()[frameInfo.frameIndex];
            vkCmdBindDescriptorSets
            (
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_COMPUTE,
                computePipelineLayout,
                0,
                1,
                &descriptorSet,
                0,
                nullptr
            );
            // noise texture
            descriptorSet = component->GetNoiseTextureDescriptorSet()[frameInfo.frameIndex];
            vkCmdBindDescriptorSets
            (
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_COMPUTE,
                computePipelineLayout,
                1,
                1,
                &descriptorSet,
                0,
                nullptr
            );

            // Compute & Dispatch
            component->Compute(frameInfo);

        }
    }

    void ComputeGrassPipeline::Render(FrameInfo& frameInfo)
    {
        // Bind Pipeline
        vkCmdBindPipeline(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        // Global Descriptor Sets (Camera)
        vkCmdBindDescriptorSets
        (
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphicsPipelineLayout,
            0,
            1,
            &descriptorManager->GetGlobalDescriptorWrap()
            ->GetDescriptorSets()[frameInfo.frameIndex],
            0,
            nullptr
        );
        // For each game object
        for (auto& gobj : gameObjects)
        {
            if (gobj->IsActive() == false) continue;
            auto computeComponent = gobj->GetComponent<Component::ComputeGrassComponent>();
            if (computeComponent->IsActive() == false) continue;
            auto meshComponent = gobj->GetComponent<Component::MeshComponent>();
            auto textureComponent = gobj->GetComponent<Component::TextureComponent>();

            SimplePushConstantData push{};
            push.modelMatrix = gobj->transform.GetModelMatrix();
            push.normalMatrix = glm::mat4(1);
            vkCmdPushConstants
            (
                frameInfo.commandBuffer,
                graphicsPipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );

            // Local Descriptors Sets
            auto ssboDescriptorSet = computeComponent->GetDescriptorSets()[frameInfo.frameIndex];
            vkCmdBindDescriptorSets
            (
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                graphicsPipelineLayout,
                1,
                1,
                &ssboDescriptorSet,
                0,
                nullptr
            );
            auto textureDescriptorSet = textureComponent->GetDescriptorSets()[frameInfo.frameIndex];
            vkCmdBindDescriptorSets
            (
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                graphicsPipelineLayout,
                2,
                1,
                &textureDescriptorSet,
                0,
                nullptr
            );

            // Draw Mesh
            // GPU instancing
            meshComponent->Bind(frameInfo.commandBuffer);
            meshComponent->InstanceDraw(frameInfo.commandBuffer, computeComponent->GetMaxGrassBladeCount());
        }
    }

    void ComputeGrassPipeline::createLayout()
    {
        createGraphicsPipelineLayout();
        createComputePipelineLayout();
    }

    void ComputeGrassPipeline::createPipeline()
    {
        createGraphicsPipeline();
        createComputePipeline();
    }

    void ComputeGrassPipeline::destroyPipeline()
    {
        vkDestroyPipeline(ihcDevice.GetDevice(), graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(ihcDevice.GetDevice(), graphicsPipelineLayout, nullptr);

        vkDestroyPipeline(ihcDevice.GetDevice(), computePipeline, nullptr);
        vkDestroyPipelineLayout(ihcDevice.GetDevice(), computePipelineLayout, nullptr);
    }

    void ComputeGrassPipeline::createGraphicsPipelineLayout()
    {
        // In GLSL, our case
        // globalDescriptorSetLayout  set 0, Binding0, UNIFORM_BUFFER
        // computeDescriptorSetLayout set 1, Binding0, UNIFORM_BUFFER
        // computeDescriptorSetLayout set 1, Binding1, STORAGE_BUFFER
        // computeDescriptorSetLayout set 1, Binding2, STORAGE_BUFFER
        // textureDescriptorSetLayout set 2, Binding0, COMBINED_IMAGE_SAMPLER

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts
        {
            descriptorManager->GetGlobalDescriptorWrap()->GetDescriptorSetLayout(),
            descriptorManager->GetComputeGrassDescriptorWrap()->GetDescriptorSetLayout(),
        	descriptorManager->GetTextureDescriptorWrap()->GetDescriptorSetLayout(),
        };


        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0; // not using separate ranges
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(ihcDevice.GetDevice(),
            &pipelineLayoutInfo, nullptr, &graphicsPipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }
    void ComputeGrassPipeline::createComputePipelineLayout()
    {
        // In GLSL, our case
        // computeDescriptorSetLayout set 0, Binding0, UNIFORM_BUFFER
        //                            set 0, Binding1, STORAGE_BUFFER
        //                            set 0, Binding2, STORAGE_BUFFER
        //                            set 1, Binding0, COMBINED_IMAGE_SAMPLER

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts
        {
            descriptorManager->GetComputeGrassDescriptorWrap()->GetDescriptorSetLayout(),
            descriptorManager->GetTextureDescriptorWrap()->GetDescriptorSetLayout(),
        };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

        if (vkCreatePipelineLayout(ihcDevice.GetDevice(),
            &pipelineLayoutInfo, nullptr, &computePipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create compute pipeline layout!");
        }
    }
    void ComputeGrassPipeline::createGraphicsPipeline()
    {
        auto vertShaderCode = readFile("Engine/assets/shaders/rendergrassvert.spv");
        auto fragShaderCode = readFile("Engine/assets/shaders/rendergrassfrag.spv");

        // Mesh particles
        auto bindingDescriptions = Vertex::getBindingDescriptions();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        // Shader stages
        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
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
        vertexInputInfo.vertexBindingDescriptionCount = 1; 
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data(); 
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        // PipelineConfig
        PipelineConfigInfo pipelineConfig{};
        IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(pipelineConfig, ihcDevice);


        pipelineConfig.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;// Mesh particles
        pipelineConfig.rasterizer.cullMode = VK_CULL_MODE_NONE;
        pipelineConfig.multisampling.sampleShadingEnable = VK_FALSE;
        pipelineConfig.multisampling.rasterizationSamples = ihcDevice.GetMsaaSamples(); //VK_SAMPLE_COUNT_1_BIT;
        pipelineConfig.colorBlendAttachment.blendEnable = VK_TRUE;
        pipelineConfig.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &pipelineConfig.inputAssembly;
        pipelineInfo.pViewportState = &pipelineConfig.viewportInfo;
        pipelineInfo.pRasterizationState = &pipelineConfig.rasterizer;
        pipelineInfo.pMultisampleState = &pipelineConfig.multisampling;
        pipelineInfo.pColorBlendState = &pipelineConfig.colorBlending;
        pipelineInfo.pDepthStencilState = &pipelineConfig.depthStencil;
        pipelineInfo.pDynamicState = &pipelineConfig.dynamicStateInfo;
        pipelineInfo.layout = graphicsPipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(ihcDevice.GetDevice(),
            VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(ihcDevice.GetDevice(), fragShaderModule, nullptr);
        vkDestroyShaderModule(ihcDevice.GetDevice(), vertShaderModule, nullptr);
    }

    void ComputeGrassPipeline::createComputePipeline()
    {
        auto computeShaderCode = readFile("Engine/assets/shaders/computegrasscomp.spv");

        VkShaderModule computeShaderModule = createShaderModule(computeShaderCode);

        VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
        computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        computeShaderStageInfo.module = computeShaderModule;
        computeShaderStageInfo.pName = "main";

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = computePipelineLayout;
        pipelineInfo.stage = computeShaderStageInfo;

        if (vkCreateComputePipelines(ihcDevice.GetDevice(),
            VK_NULL_HANDLE, 1, &pipelineInfo,
            nullptr, &computePipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create compute pipeline!");
        }

        vkDestroyShaderModule(ihcDevice.GetDevice(), computeShaderModule, nullptr);
    }

    std::vector<char> ComputeGrassPipeline::readFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        // YOU MIGHT FORGOT TO COMPILE SHADERS (SPV) IF YOU CRASH HERE
        // check if compile.bat contains your shaders
        // then run Engine/assets/shader/compile.bat
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
    VkShaderModule ComputeGrassPipeline::createShaderModule(const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(ihcDevice.GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }
}
