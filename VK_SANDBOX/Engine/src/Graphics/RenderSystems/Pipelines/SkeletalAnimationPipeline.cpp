#include "../../../pch.h"
#include "SkeletalAnimationPipeline.h"

#include "../../VKWraps/VKHelpers.h"
#include "../../VKWraps/IHCDevice.h"
#include "../../VKWraps/IHCPipeline.h"
#include "../../VKWraps/IHCDescriptorManager.h"


#include "../../VKWraps/IHCBuffer.h"
#include "../../VKWraps/IHCMesh.h"
#include "../../VKWraps/IHCTexture.h"

#include "../../../Core/Scene/Components/PipelineComponent.h"
#include "../../../Core/Scene/Components/ModelComponent.h"
#include "../../../Core/Scene/Components/IKComponent.h"
#include "../../../Core/Scene/Components/AnimatorComponent.h"
#include "../../../Core/Scene/GameObject.h"
#include "../../../Core/Time/Time.h"
#include "../../../../Engine/src/Graphics/RenderSystems/RenderSystem.h"

namespace IHCEngine::Graphics
{
    SkeletalAnimationPipeline::SkeletalAnimationPipeline(IHCDevice& device, VkRenderPass renderPass, const IHCDescriptorManager* descriptorManager)
        : CustomPipelineBase(device, renderPass, descriptorManager)
    {
        createLayout();
        createPipeline();
    }

    SkeletalAnimationPipeline::~SkeletalAnimationPipeline()
    {
        destroyPipeline();
    }

    void SkeletalAnimationPipeline::Render(FrameInfo& frameInfo)
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
            // then animated meshes
            VkDescriptorSet_T* skeletalDescriptorSet;
            SkeletalUniformBufferObject subo;
            auto animatorComponent = gobj->GetComponent<Component::AnimatorComponent>();
            auto ikComponent = gobj->GetComponent<Component::IKComponent>();

            if (animatorComponent != nullptr && animatorComponent->IsActive() && animatorComponent->HasAnimation())
            {
                // Update animation bone matrices
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
            // Has IKComponent
            else if (ikComponent != nullptr && ikComponent->IsActive()) 
            {
                // Update IK bone matrices
                ikComponent->Update();

                // Link to shader
                skeletalDescriptorSet = ikComponent->GetDescriptorSets()[frameInfo.frameIndex];
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
                subo.hasAnimation = true; // refactor in the future -> has skeletal
                const auto& boneMatrices = ikComponent->GetFinalBoneMatrices();
                size_t copySize = std::min(boneMatrices.size(), static_cast<size_t>(100));
                memcpy(subo.finalBonesMatrices, boneMatrices.data(), sizeof(glm::mat4) * copySize);
                auto buffersforwriting = ikComponent->GetBuffers()[frameInfo.frameIndex];
                buffersforwriting->WriteToBuffer(&subo);
                buffersforwriting->Flush(); // Manual flush, can comment out if using VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
            }
            else // No animation
            {
                // Use dummy to prevent vulkan errors
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
            for (const auto& mesh : modelMeshes)
            {
                MaterialData materialDataForMesh = gobjModel->GetMaterialForMesh(mesh.first);
                // Get texture descriptorSet
                VkDescriptorSet_T* descriptorSet;
                if (materialDataForMesh.diffuseMaps.size() == 0) // no texture, only color
                {
                }
                else
                {
                    descriptorSet = materialDataForMesh.diffuseMaps[0]->GetDescriptorSets()[frameInfo.frameIndex];
                    std::string textureID = materialDataForMesh.diffuseMaps[0]->GetName();
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

                // Bind Mesh and draw
                if (IHCEngine::Graphics::RenderSystem::animationMeshEnabled)
                {
                    mesh.second->Bind(frameInfo.commandBuffer);
                    mesh.second->Draw(frameInfo.commandBuffer);
                }
            }
        }
    }

    void SkeletalAnimationPipeline::createLayout()
    {
        // In GLSL, our case
		// globalDescriptorSetLayout set 0, Binding0, UNIFORM_BUFFER
		// textureDescriptorSetLayout set 1, Binding0, COMBINED_IMAGE_SAMPLER
		// skeletalDescriptorSetLayout set 2, Binding0, UNIFORM_BUFFER

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

    void SkeletalAnimationPipeline::createPipeline()
    {
        PipelineConfigInfo skeletalAnimationPipelineConfig{};
        IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(skeletalAnimationPipelineConfig, ihcDevice);
        skeletalAnimationPipelineConfig.renderPass = renderPass;
        skeletalAnimationPipelineConfig.pipelineLayout = pipelineLayout;

        pipeline = std::make_unique<IHCEngine::Graphics::IHCPipeline>
            (
                ihcDevice,
                "Engine/assets/shaders/skeletalvert.spv",
                "Engine/assets/shaders/skeletalfrag.spv",
                skeletalAnimationPipelineConfig
            );
    }

    void SkeletalAnimationPipeline::destroyPipeline()
    {
        vkDestroyPipelineLayout(ihcDevice.GetDevice(), pipelineLayout, nullptr);
    }

}
