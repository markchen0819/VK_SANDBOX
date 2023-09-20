#pragma once
// Forward declaration
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
        ~RenderSystem();

        // no duplication
        RenderSystem(const RenderSystem&) = delete;
        RenderSystem &operator=(const RenderSystem&) = delete;

        void RenderGameObjects(FrameInfo& frameInfo);

    private:

        void createDefaultPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts); // (CPU - shader stages, uniform buffers, samplers, and push constants)
        void destroyDefaultPipelineLayout();
        void createDefaultPipeline(VkRenderPass renderPass); // lifecycle not tied to renderPass, just used for creation
        void renderDefaultGraphicsPipeline(FrameInfo& frameInfo);

        IHCEngine::Graphics::IHCDevice& ihcDevice;
        VkRenderPass vkrenderpass;
        IHCDescriptorManager* descriptorManager = nullptr;

        VkPipelineLayout defaultGraphicsPipelineLayout;
        std::unique_ptr<IHCEngine::Graphics::IHCPipeline> defaultGraphicsPipeline; // basic shading

        //// Other pipelines ////
        void createCustomPipelineLayoutsAndPipelines();
        void destroyCustomPipelineLayouts();
        void createCustomPipelineLayout(VkPipelineLayout* targetPipelineLayout, std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
        void destroyCustomPipelineLayout(VkPipelineLayout* targetPipelineLayout);
        std::unique_ptr<IHCPipeline> createCustomPipeline(PipelineConfigInfo& pipelineConfigInfo, std::string vertpath, std::string fragpath);

       
        bool wireframeEnabled = false;
        std::unique_ptr<IHCEngine::Graphics::IHCPipeline> wireframePipeline; // wireframe
        void renderWireframePipeline(FrameInfo& frameInfo);
        VkPipelineLayout wireframePipelineLayout;

        std::unique_ptr<IHCEngine::Graphics::IHCPipeline> skeletalAnimationPipeline; 
        void renderSkeletalAnimationPipeline(FrameInfo& frameInfo);
        VkPipelineLayout skeletalPipelineLayout;

        // Each system can have one to many pipelines
		// Example: 
		// standardPipeline: Regular Phong shading with textures.
		// nightVisionPipeline: Everything is rendered in green shades.
		// wireframePipeline : For debugging, where all the objects are drawn
    };

}