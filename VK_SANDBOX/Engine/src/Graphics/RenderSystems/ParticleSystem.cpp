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
    computeGrassPipeline = std::make_unique<ComputeGrassPipeline>(ihcDevice, vkrenderpass, descriptorManager);
    computeFluidPipeline = std::make_unique<ComputeFluidPipeline>(ihcDevice, vkrenderpass, descriptorManager);
}


void IHCEngine::Graphics::ParticleSystem::Compute(FrameInfo& frameInfo)
{
    computeParticlePipeline->Compute(frameInfo);
    computeGrassPipeline->Compute(frameInfo);
    computeFluidPipeline->Compute(frameInfo);
}

void IHCEngine::Graphics::ParticleSystem::RenderGameObjects(FrameInfo& frameInfo)
{
    computeParticlePipeline->Render(frameInfo);
    computeGrassPipeline->Render(frameInfo);
    computeFluidPipeline->Render(frameInfo);
}

void IHCEngine::Graphics::ParticleSystem::AddGameObject(Core::GameObject* gobj, PipelineType pipelineType)
{
    switch (pipelineType)
    {
    case PipelineType::COMPUTEPARTICLE:
        computeParticlePipeline->AddGameObjectToRender(gobj); 
        break;
    case PipelineType::COMPUTEGRASS:
        computeGrassPipeline->AddGameObjectToRender(gobj);
        break;
    case PipelineType::COMPUTEFLUID:
        computeFluidPipeline->AddGameObjectToRender(gobj);
        break;
    }
}

void IHCEngine::Graphics::ParticleSystem::RemoveGameObject(Core::GameObject* gobj, PipelineType pipelineType)
{
    switch (pipelineType)
    {
    case PipelineType::COMPUTEPARTICLE:
        computeParticlePipeline->RemoveGameObjectToRender(gobj); 
        break;
    case PipelineType::COMPUTEGRASS:
        computeGrassPipeline->RemoveGameObjectToRender(gobj); 
        break;
    case PipelineType::COMPUTEFLUID:
        computeFluidPipeline->RemoveGameObjectToRender(gobj);
        break;
    }
}

