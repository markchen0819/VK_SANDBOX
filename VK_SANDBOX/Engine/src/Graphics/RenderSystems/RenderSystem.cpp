#include "../../pch.h"
#include "RenderSystem.h"

// Vulkan
#include "../VKWraps/VKHelpers.h"
#include "../VKWraps/IHCDevice.h"
#include "../VKWraps/IHCPipeline.h"
#include "../VKWraps/IHCBuffer.h"
#include "../VKWraps/IHCMesh.h"
#include "../VKWraps/IHCTexture.h"
#include "../VKWraps/IHCDescriptorManager.h"
#include "../VKWraps/DescriptorWraps/GlobalDescriptorWrap.h"
#include "../VKWraps/DescriptorWraps/TextureDescriptorWrap.h"
#include "../VKWraps/DescriptorWraps/SkeletalDescriptorWrap.h"

// Scene
#include "../../Core/Time/Time.h"
#include "../../Core/Scene/GameObject.h"

// Components
#include "../../Core/Scene/Components/MeshComponent.h"
#include "../../Core/Scene/Components/TextureComponent.h"
#include "../../Core/Scene/Components/ModelComponent.h"
#include "../../Core/Scene/Components/AnimatorComponent.h"
#include "../../Core/Scene/Components/IKComponent.h"
#include "../../Core/Scene/Components/PipelineComponent.h"
#include "../../Core/Scene/Components/LineRendererComponent.h"

IHCEngine::Graphics::RenderSystem::RenderSystem(IHCDevice& device, VkRenderPass renderPass, IHCDescriptorManager* descriptorManager)
    :   ihcDevice{ device },
		vkrenderpass{ renderPass },
		descriptorManager{ descriptorManager }
{
    auto layout = descriptorManager->GetDefaultDescriptorSetLayoutsForBasicRenderSystem();
    createDefaultPipelineLayout(layout);
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
    // Skeletal animation pipeline
    PipelineConfigInfo skeletalAnimationPipelineConfig{};
    // In GLSL, our case
	// globalDescriptorSetLayout set 0, Binding0, UNIFORM_BUFFER
	// textureDescriptorSetLayout set 1, Binding0, COMBINED_IMAGE_SAMPLER
	// skeletalDescriptorSetLayout set 2, Binding0, UNIFORM_BUFFER
	std::vector<VkDescriptorSetLayout> layouts {
        descriptorManager->GetGlobalDescriptorWrap()->GetDescriptorSetLayout(),
        descriptorManager->GetTextureDescriptorWrap()->GetDescriptorSetLayout(),
        descriptorManager->GetSkeletalDescriptorWrap()->GetDescriptorSetLayout(),
    };
    IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(skeletalAnimationPipelineConfig, ihcDevice);
    createCustomPipelineLayout(&skeletalPipelineLayout, layouts);
    // config info
	skeletalAnimationPipelineConfig.renderPass = vkrenderpass;
    skeletalAnimationPipelineConfig.pipelineLayout = skeletalPipelineLayout; // use default
    skeletalAnimationPipeline = createCustomPipeline(
        skeletalAnimationPipelineConfig,
        "Engine/assets/shaders/skeletalvert.spv",
        "Engine/assets/shaders/skeletalfrag.spv");

    // Wire frame
    // use same layout as skeletal
    createCustomPipelineLayout(&wireframePipelineLayout, layouts);
    // config info
    PipelineConfigInfo wireframePipelineConfig{};
    IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(wireframePipelineConfig, ihcDevice);
    wireframePipelineConfig.renderPass = vkrenderpass;
    wireframePipelineConfig.pipelineLayout = wireframePipelineLayout; // use default
    wireframePipelineConfig.rasterizer.polygonMode = VK_POLYGON_MODE_LINE; // This is the crucial line for wireframe mode.
    wireframePipelineConfig.rasterizer.lineWidth = 1.0f; // You can adjust this as needed
    wireframePipelineConfig.rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // Or whatever culling mode you prefer

	wireframePipeline = createCustomPipeline(wireframePipelineConfig,
        "Engine/assets/shaders/skeletalvert.spv",
        "Engine/assets/shaders/skeletalfrag.spv");

    // Debug Bone
    std::vector<VkDescriptorSetLayout> boneLayouts {
        descriptorManager->GetGlobalDescriptorWrap()->GetDescriptorSetLayout(),
    };
    createCustomPipelineLayout(&debugBonePipelineLayout, boneLayouts);
    PipelineConfigInfo debugBonePipelineConfig{};
    IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(debugBonePipelineConfig, ihcDevice);
    debugBonePipelineConfig.renderPass = vkrenderpass;
    debugBonePipelineConfig.pipelineLayout = debugBonePipelineLayout;
    debugBonePipelineConfig.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    debugBonePipelineConfig.rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
    debugBonePipelineConfig.rasterizer.lineWidth = 1.0f;
    debugBonePipelineConfig.depthStencil.depthTestEnable = VK_FALSE;
    debugBonePipelineConfig.depthStencil.depthWriteEnable = VK_FALSE;
    debugBonePipeline = createCustomPipeline(debugBonePipelineConfig,
        "Engine/assets/shaders/debugbonevert.spv",
        "Engine/assets/shaders/debugbonefrag.spv");


    // LineRenderer
    std::vector<VkDescriptorSetLayout> lineRendererLayouts
	{
        descriptorManager->GetGlobalDescriptorWrap()->GetDescriptorSetLayout(),
    };
    createCustomPipelineLayout(&lineRendererPipelineLayout, lineRendererLayouts);
    PipelineConfigInfo lineRendererPipelineConfig{};
    IHCEngine::Graphics::IHCPipeline::DefaultPipelineConfigInfo(lineRendererPipelineConfig, ihcDevice);
    lineRendererPipelineConfig.renderPass = vkrenderpass;
    lineRendererPipelineConfig.pipelineLayout = lineRendererPipelineLayout;
    lineRendererPipelineConfig.inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    lineRendererPipelineConfig.rasterizer.lineWidth = 3.0f;
    lineRendererPipeline = createCustomPipeline(lineRendererPipelineConfig,
        "Engine/assets/shaders/linerenderervert.spv",
        "Engine/assets/shaders/linerendererfrag.spv");



}
void IHCEngine::Graphics::RenderSystem::destroyCustomPipelineLayouts()
{
    //vkDestroyPipelineLayout(ihcDevice.GetDevice(), PLACEHOLDER, nullptr);
    vkDestroyPipelineLayout(ihcDevice.GetDevice(), skeletalPipelineLayout, nullptr);
    vkDestroyPipelineLayout(ihcDevice.GetDevice(), wireframePipelineLayout, nullptr);
    vkDestroyPipelineLayout(ihcDevice.GetDevice(), debugBonePipelineLayout, nullptr);
    vkDestroyPipelineLayout(ihcDevice.GetDevice(), lineRendererPipelineLayout, nullptr);
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
        if (debugBonesEnabled)
        {
            renderDebugBonePipeline(frameInfo);
        }
    }
    else
    {
        renderDefaultGraphicsPipeline(frameInfo);
        renderSkeletalAnimationPipeline(frameInfo);
        renderLineRendererPipeline(frameInfo);
        if(debugBonesEnabled)
        {
            renderDebugBonePipeline(frameInfo);
        }
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
        &frameInfo.descriptorManager->GetGlobalDescriptorWrap()
        ->GetDescriptorSets()[frameInfo.frameIndex],
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
        if (gobj->IsActive() == false) continue;
        // Only render the ones specifying this pipeline
        auto pipelineComponent = gobj->GetComponent<Component::PipelineComponent>();
        if (pipelineComponent == nullptr ||
            pipelineComponent->GetPipelineType() !=
            Component::PipelineType::DEFAULT) continue;

        // Has mesh & texture (model in a different pipeline)
        if (!gobj->HasComponent<Component::MeshComponent>()) continue;
        if (!gobj->HasComponent<Component::TextureComponent>()) continue;

        // Bind Local Descriptor Set
        // Common case: Material Textures (Texture, NormalMap, AO), Material Properties, Transform Matrices for Skinned Animations
        // Our case: Texture
        auto descriptorSet = gobj->GetComponent<Component::TextureComponent>()->GetDescriptorSets()[frameInfo.frameIndex];
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
        // Bind Mesh and Draw
        gobj->GetComponent<Component::MeshComponent>()->Bind(frameInfo.commandBuffer);
        gobj->GetComponent<Component::MeshComponent>()->Draw(frameInfo.commandBuffer);
    }
}
void IHCEngine::Graphics::RenderSystem::renderWireframePipeline(FrameInfo& frameInfo)
{
    // Bind Pipeline 
    wireframePipeline->Bind(frameInfo.commandBuffer);
    SkeletalUniformBufferObject subo;
    vkCmdBindDescriptorSets
    (
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        wireframePipelineLayout,
        0,
        1,
        &frameInfo.descriptorManager->GetGlobalDescriptorWrap()->GetDescriptorSets()[frameInfo.frameIndex],
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
                animatorComponent->UpdateAnimation(frameInfo.frameTime);
                // Link to shader
                skeletalDescriptorSet = animatorComponent->GetDescriptorSets()[frameInfo.frameIndex];
                vkCmdBindDescriptorSets
                (
                    frameInfo.commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    wireframePipelineLayout,
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
                    wireframePipelineLayout,
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
                    wireframePipelineLayout,
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
                    wireframePipelineLayout,
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

            //// why is texture still needed?
            //auto descriptorSet = gobj->GetComponent<Component::TextureComponent>()->GetDescriptorSets()[frameInfo.frameIndex];
            //vkCmdBindDescriptorSets
            //(
            //    frameInfo.commandBuffer,
            //    VK_PIPELINE_BIND_POINT_GRAPHICS,
            //    defaultGraphicsPipelineLayout,
            //    1,
            //    1,
            //    &descriptorSet,
            //    0,
            //    nullptr
            //);
            // use dummy to prevent vulkan errors
            auto skeletalDescriptorSet = descriptorManager->GetSkeletalDescriptorWrap()->GetDummySkeletalDescriptorSet();
            vkCmdBindDescriptorSets
            (
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                skeletalPipelineLayout,
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
                wireframePipelineLayout,
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
void IHCEngine::Graphics::RenderSystem::renderSkeletalAnimationPipeline(IHCEngine::Graphics::FrameInfo& frameInfo)
{
    // Bind Pipeline 
    skeletalAnimationPipeline->Bind(frameInfo.commandBuffer);
    SkeletalUniformBufferObject subo;
    // global Descriptor Sets (Camera)
    vkCmdBindDescriptorSets
    (
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        skeletalPipelineLayout,
        0,
        1,
        &frameInfo.descriptorManager->GetGlobalDescriptorWrap()
        ->GetDescriptorSets()[frameInfo.frameIndex],
        0,
        nullptr
    );
    // For each game object
    for (auto& g : frameInfo.gameObjects)
    {
        IHCEngine::Core::GameObject* gobj = g.second;
        if (gobj->IsActive() == false) continue;
        // Only render the ones specifying this pipeline
        auto pipelineComponent = gobj->GetComponent<Component::PipelineComponent>();
        if (pipelineComponent==nullptr ||
            pipelineComponent->GetPipelineType()!= 
            Component::PipelineType::SKELETAL) continue;
   
        // SkeletalAnimationPipeline Requires a model
        if (!gobj->HasComponent<Component::ModelComponent>()) continue;

        // Has animation
        VkDescriptorSet_T* skeletalDescriptorSet;
        auto animatorComponent = gobj->GetComponent<Component::AnimatorComponent>();
        auto ikComponent = gobj->GetComponent<Component::IKComponent>();
        if( animatorComponent!=nullptr && animatorComponent->HasAnimation())
        {
            // Update the animation
            animatorComponent->UpdateAnimation(frameInfo.frameTime);
            // Link to shader
            skeletalDescriptorSet = animatorComponent->GetDescriptorSets()[frameInfo.frameIndex];
            vkCmdBindDescriptorSets
            (
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                skeletalPipelineLayout,
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
        else if (ikComponent != nullptr) // Has IK
        {
            ikComponent->Update();
            // Link to shader
            skeletalDescriptorSet = ikComponent->GetDescriptorSets()[frameInfo.frameIndex];
            vkCmdBindDescriptorSets
            (
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                skeletalPipelineLayout,
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
            // use dummy to prevent vulkan errors
            skeletalDescriptorSet = descriptorManager->GetSkeletalDescriptorWrap()->GetDummySkeletalDescriptorSet();
            vkCmdBindDescriptorSets
            (
                frameInfo.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                skeletalPipelineLayout,
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
                skeletalPipelineLayout,
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
                skeletalPipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );
            // Bind Mesh and draw
            if (animationMeshEnabled)
            {
                mesh.second->Bind(frameInfo.commandBuffer);
                mesh.second->Draw(frameInfo.commandBuffer);
            }
        }
    }
}
void IHCEngine::Graphics::RenderSystem::renderDebugBonePipeline(FrameInfo& frameInfo)
{
    // Bind Pipeline 
    debugBonePipeline->Bind(frameInfo.commandBuffer);
    // global Descriptor Sets (Camera)
    vkCmdBindDescriptorSets
    (
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        skeletalPipelineLayout,
        0,
        1,
        &frameInfo.descriptorManager->GetGlobalDescriptorWrap()
        ->GetDescriptorSets()[frameInfo.frameIndex],
        0,
        nullptr
    );
    // For each game object
    for (auto& g : frameInfo.gameObjects)
    {
        IHCEngine::Core::GameObject* gobj = g.second;
        if (gobj->IsActive() == false) continue;
        // Only render the ones specifying this pipeline
        auto pipelineComponent = gobj->GetComponent<Component::PipelineComponent>();
        if (pipelineComponent == nullptr ||
            pipelineComponent->GetPipelineType() !=
            Component::PipelineType::SKELETAL) continue;

        // SkeletalAnimationPipeline Requires a model
        if (!gobj->HasComponent<Component::ModelComponent>()) continue;

        // Has animation
        VkDescriptorSet_T* skeletalDescriptorSet;
        auto animatorComponent = gobj->GetComponent<Component::AnimatorComponent>();
        auto ikComponent = gobj->GetComponent<Component::IKComponent>();
    	if (animatorComponent != nullptr && animatorComponent->HasAnimation())
        {
            // Draw Debug Bones
            SimplePushConstantData push{};
            push.modelMatrix = gobj->transform.GetModelMatrix();
            push.normalMatrix = glm::mat4(1);
            vkCmdPushConstants
            (
                frameInfo.commandBuffer,
                skeletalPipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );
            animatorComponent->UpdateDebugBoneBuffer(frameInfo);
            animatorComponent->DrawDebugBoneBuffer(frameInfo);
        }
        else if (ikComponent != nullptr) // Has IK
        {
            // Draw Debug Bones
            SimplePushConstantData push{};
            push.modelMatrix = gobj->transform.GetModelMatrix();
            push.normalMatrix = glm::mat4(1);
            vkCmdPushConstants
            (
                frameInfo.commandBuffer,
                skeletalPipelineLayout,
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
void IHCEngine::Graphics::RenderSystem::renderLineRendererPipeline(FrameInfo& frameInfo)
{
    // Bind Pipeline 
    lineRendererPipeline->Bind(frameInfo.commandBuffer);
    // global Descriptor Sets (Camera)
    vkCmdBindDescriptorSets
    (
        frameInfo.commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        lineRendererPipelineLayout,
        0,
        1,
        &frameInfo.descriptorManager->GetGlobalDescriptorWrap()
        ->GetDescriptorSets()[frameInfo.frameIndex],
        0,
        nullptr
    );
    // For each game object
    for (auto& g : frameInfo.gameObjects)
    {
        IHCEngine::Core::GameObject* gobj = g.second;
        if (gobj->IsActive() == false) continue;
        // Only render the ones specifying this pipeline


        if(!gobj->HasComponent<Component::LineRendererComponent>()) continue;
        auto lineRendererComponent = gobj->GetComponent<Component::LineRendererComponent>();

        SimplePushConstantData push{};
        push.modelMatrix = gobj->transform.GetModelMatrix();
        push.normalMatrix = glm::mat4(1);
        vkCmdPushConstants
        (
            frameInfo.commandBuffer,
            skeletalPipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push
        );
        lineRendererComponent->Draw(frameInfo);
    }
}

#pragma endregion
