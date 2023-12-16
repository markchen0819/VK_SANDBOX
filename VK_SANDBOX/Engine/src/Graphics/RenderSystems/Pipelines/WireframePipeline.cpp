#include "../../../pch.h"
#include "WireframePipeline.h"

#include "../../VKWraps/VKHelpers.h"
#include "../../VKWraps/IHCDevice.h"
#include "../../VKWraps/IHCPipeline.h"
#include "../../VKWraps/IHCDescriptorManager.h"


#include "../../VKWraps/IHCBuffer.h"
#include "../../VKWraps/IHCMesh.h"
#include "../../VKWraps/IHCTexture.h"

#include "../../../Core/Scene/Components/TextureComponent.h"
#include "../../../Core/Scene/Components/ModelComponent.h"
#include "../../../Core/Scene/Components/MeshComponent.h"
#include "../../../Core/Scene/Components/AnimatorComponent.h"
#include "../../../Core/Scene/GameObject.h"
#include "../../../Core/Time/Time.h"
#include "../../../../Engine/src/Graphics/RenderSystems/RenderSystem.h"

namespace IHCEngine::Graphics
{
    WireframePipeline::WireframePipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager)
        : CustomPipelineBase(device, renderPass, descriptorManager)
    {
        createLayout();
        createPipeline();
    }

    WireframePipeline::~WireframePipeline()
    {
        destroyPipeline();
    }

    void WireframePipeline::Render(FrameInfo& frameInfo)
    {
        // Bind Pipeline 
        pipeline->Bind(frameInfo.commandBuffer);
        SkeletalUniformBufferObject subo;
        vkCmdBindDescriptorSets
        (
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &descriptorManager->GetGlobalDescriptorWrap()->GetDescriptorSets()[frameInfo.frameIndex],
            0,
            nullptr
        );

        for (auto& g : frameInfo.gameObjects)
        {
            IHCEngine::Core::GameObject* gobj = g.second;
            if (gobj->IsActive() == false) continue;
            if (gobj->HasComponent<Component::ModelComponent>())
            {
                // Has animation
                VkDescriptorSet_T* skeletalDescriptorSet;
                auto animatorComponent = gobj->GetComponent<Component::AnimatorComponent>();
                if (animatorComponent == nullptr || animatorComponent->HasAnimation())
                {
                    // Update the animation
                    float dt = IHCEngine::Core::Time::GetDeltaTime();
                    animatorComponent->UpdateAnimation(dt);
                    // Link to shader
                    skeletalDescriptorSet = animatorComponent->GetDescriptorSets()[frameInfo.frameIndex];
                    vkCmdBindDescriptorSets
                    (
                        frameInfo.commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        pipelineLayout,
                        2,
                        1,
                        &skeletalDescriptorSet,
                        0,
                        nullptr
                    );
                    // Write to buffer shader points to
                    subo.hasAnimation = true;
                    const auto& boneMatrices = animatorComponent->GetFinalBoneMatrices();
                    size_t copySize = std::min(boneMatrices.size(), static_cast<size_t>(100));
                    memcpy(subo.finalBonesMatrices, boneMatrices.data(), sizeof(glm::mat4) * copySize);
                    auto buffersforwriting = animatorComponent->GetBuffers()[frameInfo.frameIndex];
                    buffersforwriting->WriteToBuffer(&subo);
                    buffersforwriting->Flush(); // Manual flush, can comment out if using VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
                }
                else // No animation
                {
                    // use dummy to prevent vulkan errors
                    skeletalDescriptorSet = descriptorManager->GetSkeletalDescriptorWrap()->GetDummySkeletalDescriptorSet();
                    vkCmdBindDescriptorSets
                    (
                        frameInfo.commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        pipelineLayout,
                        2,
                        1,
                        &skeletalDescriptorSet,
                        0,
                        nullptr
                    );
                    subo.hasAnimation = false;
                    size_t copySize = static_cast<size_t>(100);
                    for (size_t i = 0; i < copySize; ++i)
                    {
                        subo.finalBonesMatrices[i] = glm::mat4(1.0);
                    }
                    auto buffersforwriting = descriptorManager->GetSkeletalDescriptorWrap()->GetDummySkeletalUBO();
                    buffersforwriting->WriteToBuffer(&subo);
                    buffersforwriting->Flush();
                }

                // Bind and Draw the Model meshes
                auto gobjModel = gobj->GetComponent<Component::ModelComponent>();
                auto modelMeshes = gobjModel->GetMeshes();
                for (const auto& mesh : modelMeshes)
                {
                    MaterialData materialDataForMesh = gobjModel->GetMaterialForMesh(mesh.first);
                    // Get texture descriptorSet
                    VkDescriptorSet_T* descriptorSet;
                    if (materialDataForMesh.diffuseMaps.size() == 0) // no texture, only color
                    {
                        // descriptorSet = frameInfo.descriptorManager->GetTextureToDescriptorSetsMap()["plainTexture"][frameInfo.frameIndex];
                    }
                    else
                    {
                        descriptorSet = materialDataForMesh.diffuseMaps[0]->GetDescriptorSets()[frameInfo.frameIndex];
                        std::string textureID = materialDataForMesh.diffuseMaps[0]->GetName();
                        //descriptorSet = frameInfo.descriptorManager->GetTextureToDescriptorSetsMap()[textureID][frameInfo.frameIndex];
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
                    // Bind Mesh and draw
                    mesh.second->Bind(frameInfo.commandBuffer);
                    mesh.second->Draw(frameInfo.commandBuffer);
                }

            }
            else if (gobj->HasComponent<Component::MeshComponent>())
            {
                // use dummy to prevent vulkan errors
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
                // use dummy to prevent vulkan errors
                auto skeletalDescriptorSet = descriptorManager->GetSkeletalDescriptorWrap()->GetDummySkeletalDescriptorSet();
                vkCmdBindDescriptorSets
                (
                    frameInfo.commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipelineLayout,
                    2,
                    1,
                    &skeletalDescriptorSet,
                    0,
                    nullptr
                );
                SkeletalUniformBufferObject subo;
                subo.hasAnimation = false;
                size_t copySize = static_cast<size_t>(100);
                for (size_t i = 0; i < copySize; ++i)
                {
                    subo.finalBonesMatrices[i] = glm::mat4(1.0);
                }
                auto buffersforwriting = descriptorManager->GetSkeletalDescriptorWrap()->GetDummySkeletalUBO();
                buffersforwriting->WriteToBuffer(&subo);
                buffersforwriting->Flush();

                // 
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
                // Bind Mesh and Draw
                gobj->GetComponent<Component::MeshComponent>()->Bind(frameInfo.commandBuffer);
                gobj->GetComponent<Component::MeshComponent>()->Draw(frameInfo.commandBuffer);
            }

        }
    }

    void WireframePipeline::createLayout()
    {
        // In GLSL, our case
        // globalDescriptorSetLayout set 0, Binding0, UNIFORM_BUFFER
        // textureDescriptorSetLayout set 1, Binding0, COMBINED_IMAGE_SAMPLER
        // skeletalDescriptorSetLayout set 2, Binding0, UNIFORM_BUFFER

         // use same layout as skeletal

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts
        {
            descriptorManager->GetGlobalDescriptorWrap()->GetDescriptorSetLayout(),
        	descriptorManager->GetTextureDescriptorWrap()->GetDescriptorSetLayout(),
        	descriptorManager->GetSkeletalDescriptorWrap()->GetDescriptorSetLayout(),
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

    void WireframePipeline::createPipeline()
    {
        PipelineConfigInfo wireframePipelineConfig{};
        IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(wireframePipelineConfig, ihcDevice);
        wireframePipelineConfig.renderPass = renderPass;
        wireframePipelineConfig.pipelineLayout = pipelineLayout;
        wireframePipelineConfig.rasterizer.polygonMode = VK_POLYGON_MODE_LINE; // This is the crucial line for wireframe mode.
        wireframePipelineConfig.rasterizer.lineWidth = 1.0f; // You can adjust this as needed
        wireframePipelineConfig.rasterizer.cullMode = VK_CULL_MODE_NONE; // Or whatever culling mode you prefer

        pipeline = std::make_unique<IHCEngine::Graphics::IHCPipeline>
            (
                ihcDevice,
                "Engine/assets/shaders/skeletalvert.spv",
                "Engine/assets/shaders/skeletalfrag.spv",
                wireframePipelineConfig
            );
    }

    void WireframePipeline::destroyPipeline()
    {
        vkDestroyPipelineLayout(ihcDevice.GetDevice(), pipelineLayout, nullptr);
    }

}
