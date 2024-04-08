#pragma once
#include "Pipelines/PipelineType.h"
#include "Pipelines/ComputeParticlePipeline.h"
#include "Pipelines/ComputeGrassPipeline.h"
#include "Pipelines/ComputeFluidPipeline.h"

namespace IHCEngine::Graphics
{
    class IHCDescriptorManager;
    struct FrameInfo;
    class IHCDevice;
    class IHCPipeline;
    struct PipelineConfigInfo;
}

namespace IHCEngine::Graphics
{
    class ParticleSystem
    {
    public:

        ParticleSystem(IHCDevice& device, VkRenderPass renderPass, IHCDescriptorManager* descriptorManager);
        ~ParticleSystem() = default;

        // no duplication
        ParticleSystem(const ParticleSystem&) = delete;
        ParticleSystem& operator=(const ParticleSystem&) = delete;

        void Compute(FrameInfo& frameInfo);
        void RenderGameObjects(FrameInfo& frameInfo);

        void AddGameObject(Core::GameObject* gobj, PipelineType pipelineType);
        void RemoveGameObject(Core::GameObject* gobj, PipelineType pipelineType);

    private:

        IHCEngine::Graphics::IHCDevice& ihcDevice;
        VkRenderPass vkrenderpass;
        IHCDescriptorManager* descriptorManager = nullptr;

        // compute & graphics pipeline
        std::unique_ptr<IHCEngine::Graphics::ComputeParticlePipeline> computeParticlePipeline;
        std::unique_ptr<IHCEngine::Graphics::ComputeGrassPipeline> computeGrassPipeline;
        std::unique_ptr<IHCEngine::Graphics::ComputeFluidPipeline> computeFluidPipeline;
    };

}