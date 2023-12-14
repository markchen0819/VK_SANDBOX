#pragma once

#include "Pipelines/DefaultPipeline.h"
#include "Pipelines/LineRendererPipeline.h"
#include "Pipelines/DebugBonePipeline.h"
#include "Pipelines/SkeletalAnimationPipeline.h"
#include "Pipelines/WireframePipeline.h"

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
    class RenderSystem
    {
    public:

        RenderSystem(IHCDevice& device, VkRenderPass renderPass, IHCDescriptorManager* descriptorManager);
        ~RenderSystem() = default;

        // no duplication
        RenderSystem(const RenderSystem&) = delete;
        RenderSystem &operator=(const RenderSystem&) = delete;

        void RenderGameObjects(FrameInfo& frameInfo);

        inline static bool wireframeEnabled = false;
        inline static bool debugBonesEnabled = true;
        inline static bool animationMeshEnabled = true;

    private:

        IHCEngine::Graphics::IHCDevice& ihcDevice;
        VkRenderPass vkrenderpass;
        IHCDescriptorManager* descriptorManager = nullptr;

        std::unique_ptr<IHCEngine::Graphics::DefaultPipeline> defaultPipeline; // basic shading
        //// Other pipelines ////
        std::unique_ptr <IHCEngine::Graphics::WireframePipeline> wireframePipeline;
        std::unique_ptr <IHCEngine::Graphics::SkeletalAnimationPipeline> skeletalAnimationPipeline;
        std::unique_ptr <IHCEngine::Graphics::DebugBonePipeline> debugBonePipeline;
        std::unique_ptr <IHCEngine::Graphics::LineRendererPipeline> lineRendererPipeline;
    };

}