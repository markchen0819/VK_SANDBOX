#pragma once
#include "Pipelines/ComputeParticlePipeline.h"


//
//namespace IHCEngine::Core
//{
//    class GameObject;
//}
//
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

        void RenderGameObjects(FrameInfo& frameInfo);

        //void AddGameObjectToRender(Core::GameObject* gobj, PipelineType pipelineType);
        //void RemoveGameObjectToRender(Core::GameObject* gobj, PipelineType pipelineType);

    private:

        IHCEngine::Graphics::IHCDevice& ihcDevice;
        VkRenderPass vkrenderpass;
        IHCDescriptorManager* descriptorManager = nullptr;

        std::unique_ptr<IHCEngine::Graphics::ComputeParticlePipeline> computeParticlePipeline; // basic shading
        //std::unique_ptr<IHCEngine::Graphics::DefaultPipeline> defaultPipeline; // basic shading
        ////// Other pipelines ////
        //std::unique_ptr <IHCEngine::Graphics::WireframePipeline> wireframePipeline;
        //std::unique_ptr <IHCEngine::Graphics::SkeletalAnimationPipeline> skeletalAnimationPipeline;
        //std::unique_ptr <IHCEngine::Graphics::DebugBonePipeline> debugBonePipeline;
        //std::unique_ptr <IHCEngine::Graphics::LineRendererPipeline> lineRendererPipeline;
    };

}