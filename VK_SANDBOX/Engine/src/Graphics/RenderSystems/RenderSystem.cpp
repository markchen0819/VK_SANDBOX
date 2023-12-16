#include "../../pch.h"
#include "RenderSystem.h"

// Vulkan
#include "../VKWraps/VKHelpers.h"
#include "../VKWraps/IHCDevice.h"
#include "../VKWraps/IHCDescriptorManager.h"


IHCEngine::Graphics::RenderSystem::RenderSystem(IHCDevice& device, VkRenderPass renderPass, IHCDescriptorManager* descriptorManager)
    :   ihcDevice{ device },
		vkrenderpass{ renderPass },
		descriptorManager{ descriptorManager }
{
    defaultPipeline = std::make_unique<DefaultPipeline>(ihcDevice, vkrenderpass, descriptorManager);
    wireframePipeline = std::make_unique<WireframePipeline>(ihcDevice, vkrenderpass, descriptorManager);
    skeletalAnimationPipeline = std::make_unique<SkeletalAnimationPipeline>(ihcDevice, vkrenderpass, descriptorManager);
    debugBonePipeline = std::make_unique<DebugBonePipeline>(ihcDevice, vkrenderpass, descriptorManager);
    lineRendererPipeline = std::make_unique<LineRendererPipeline>(ihcDevice, vkrenderpass, descriptorManager);
}


void IHCEngine::Graphics::RenderSystem::RenderGameObjects(FrameInfo& frameInfo)
{
    if (wireframeEnabled)
    {
        wireframePipeline->Render(frameInfo);
        if (debugBonesEnabled)
        {
            debugBonePipeline->Render(frameInfo);
        }
    }
    else
    {
        defaultPipeline->Render(frameInfo);

        skeletalAnimationPipeline->Render(frameInfo);

        lineRendererPipeline->Render(frameInfo);
        if(debugBonesEnabled)
        {
            debugBonePipeline->Render(frameInfo);
        }
    }
}

void IHCEngine::Graphics::RenderSystem::AddGameObjectToRender(Core::GameObject* gobj, PipelineType pipelineType)
{
    switch(pipelineType)
    {
    case PipelineType::DEFAULT:
        defaultPipeline->AddGameObjectToRender(gobj);
        break;
    case PipelineType::WIREFRAME:
        break;
    case PipelineType::SKELETAL:
        break;
    case PipelineType::DEBUGBONE:
        break;
    case PipelineType::LINERENDERER:
        lineRendererPipeline->AddGameObjectToRender(gobj);
        break;

    }
}

void IHCEngine::Graphics::RenderSystem::RemoveGameObjectToRender(Core::GameObject* gobj, PipelineType pipelineType)
{
    switch (pipelineType)
    {
    case PipelineType::DEFAULT:
        defaultPipeline->RemoveGameObjectToRender(gobj);
        break;
    case PipelineType::WIREFRAME:
        break;
    case PipelineType::SKELETAL:
        break;
    case PipelineType::DEBUGBONE:
        break;
    case PipelineType::LINERENDERER:
        lineRendererPipeline->RemoveGameObjectToRender(gobj);
        break;

    }
}
