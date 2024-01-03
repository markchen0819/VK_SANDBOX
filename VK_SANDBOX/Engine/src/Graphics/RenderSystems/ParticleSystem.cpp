#include "../../pch.h"
#include "ParticleSystem.h"
// Vulkan
#include "../VKWraps/VKHelpers.h"
#include "../VKWraps/IHCDevice.h"
#include "../VKWraps/IHCDescriptorManager.h"


IHCEngine::Graphics::ParticleSystem::ParticleSystem(IHCDevice& device, VkRenderPass renderPass, IHCDescriptorManager* descriptorManager)
    : ihcDevice{ device },
    vkrenderpass{ renderPass },
    descriptorManager{ descriptorManager }
{
    computeParticlePipeline = std::make_unique<ComputeParticlePipeline>(ihcDevice, vkrenderpass, descriptorManager);
    //defaultPipeline = std::make_unique<DefaultPipeline>(ihcDevice, vkrenderpass, descriptorManager);
    //wireframePipeline = std::make_unique<WireframePipeline>(ihcDevice, vkrenderpass, descriptorManager);
    //skeletalAnimationPipeline = std::make_unique<SkeletalAnimationPipeline>(ihcDevice, vkrenderpass, descriptorManager);
    //debugBonePipeline = std::make_unique<DebugBonePipeline>(ihcDevice, vkrenderpass, descriptorManager);
    //lineRendererPipeline = std::make_unique<LineRendererPipeline>(ihcDevice, vkrenderpass, descriptorManager);
}


void IHCEngine::Graphics::ParticleSystem::Compute(FrameInfo& frameInfo)
{
    computeParticlePipeline->Compute(frameInfo);
}

void IHCEngine::Graphics::ParticleSystem::RenderGameObjects(FrameInfo& frameInfo)
{
    computeParticlePipeline->Render(frameInfo);
}

void IHCEngine::Graphics::ParticleSystem::AddGameObject(Core::GameObject* gobj, PipelineType pipelineType)
{
    switch (pipelineType)
    {
    case PipelineType::COMPUTEPATRICLE:
        computeParticlePipeline->AddGameObjectToRender(gobj); // & compute
        break;

    }
}

void IHCEngine::Graphics::ParticleSystem::RemoveGameObject(Core::GameObject* gobj, PipelineType pipelineType)
{
    switch (pipelineType)
    {
    case PipelineType::COMPUTEPATRICLE:
        computeParticlePipeline->RemoveGameObjectToRender(gobj); // & compute
        break;

    }
}

