#pragma once
#include "../pch.h"
#include <stb_image.h>
#include "VKHelpers.h"
#include "IHCDevice.h"
#include "IHCPipeline.h"
#include "IHCModel.h"
#include "../Core/GameObject.h"


namespace IHCEngine::Graphics
{
    class RenderSystem
    {
    public:

        RenderSystem(IHCDevice& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
        ~RenderSystem();

        // no duplication
        RenderSystem(const RenderSystem&) = delete;
        RenderSystem &operator=(const RenderSystem&) = delete;

        void RenderGameObjects(FrameInfo& frameInfo, std::unordered_map<IHCEngine::Core::GameObject*, VkDescriptorSet> gameObjectToDescriptorSet);

    private:

        void createPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts); // (CPU - shader stages, uniform buffers, samplers, and push constants)
        void destroyPipelineLayout();
        void createPipeline(VkRenderPass renderPass); // lifecycle not tied to renderPass, just used for creation

        IHCEngine::Graphics::IHCDevice& ihcDevice;
        VkPipelineLayout pipelineLayout;
        std::unique_ptr<IHCEngine::Graphics::IHCPipeline> ihcPipeline; // basic shading


        ////// Create other pipelines //////
        // Each system can have one to many pipelines
        // Example: 
        // standardPipeline: Regular Phong shading with textures.
        // nightVisionPipeline: Everything is rendered in green shades.
        // wireframePipeline : For debugging, where all the objects are drawn

        bool wireframeEnabled = false;
        void createWireFramePipeline(VkRenderPass renderPass);
        std::unique_ptr<IHCEngine::Graphics::IHCPipeline> wireframePipeline; // wireframe


    };

}