#pragma once
#include "IHCDescriptors.h"

namespace IHCEngine::Graphics
{
	class Animator;
	class IHCTexture;
	class IHCDevice;
	class IHCBuffer;
    class IHCDescriptorPool; // memory pool
    class IHCDescriptorSetLayout;
    class IHCDescriptorWriter;

    // For accessing between shader(pipeline) and resources
	class IHCDescriptorManager
	{
	public:

        IHCDescriptorManager(IHCDevice& ihcDevice);
        ~IHCDescriptorManager()=default;

        // Get Layouts for pipeline creation
        std::vector<VkDescriptorSetLayout> GetDefaultDescriptorSetLayoutsForBasicRenderSystem();
        // Add GetCustomDescriptorSetLayouts for different pipeline creation
        VkDescriptorSetLayout GetGlobalDescriptorSetLayouts() { return globalDescriptorSetLayout->GetDescriptorSetLayout(); }
        VkDescriptorSetLayout GetTextureDescriptorSetLayouts() { return textureDescriptorSetLayout->GetDescriptorSetLayout(); }
        VkDescriptorSetLayout GetSkeletalDescriptorSetLayouts() { return skeletalDescriptorSetLayout->GetDescriptorSetLayout(); }


        // Get Resources to write into and store
        std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>>& GetGlobalUBOs() { return globalUBOs; }
        // Get DescriptorSets to bind to pipeline slot for access (set up shader binding)
        std::vector<VkDescriptorSet> GetGlobalDescriptorSets() { return globalDescriptorSets; }

        void AllocateTextureDescriptorSetForTexture(IHCTexture* texture);
        void DeallocateTextureDescriptorSetForTexture(IHCTexture* texture);
        void AllocateSkeletalDescriptorSetForAnimator(Animator* animator);
        void DeallocateSkeletalDescriptorSetForAnimator(Animator* animator);

        VkDescriptorSet GetDummySkeletalDescriptorSet() { return dummySkeletalDescriptorSet; }
        IHCBuffer* GetDummySkeletalUBO() { return dummySkeletalUBO.get(); }

	private:
        void createDescriptorSetLayouts();
        void initPool();
        void createGlobalUniformBuffers();
        void createCustomUniformBuffers();
        void allocateGlobalDescriptorSets();
       

        const int TEXTURE_COUNT_LIMIT = 200;
        const int SKELETAL_COUNT_LIMIT = 20;

        IHCDevice& ihcDevice;
        std::unique_ptr<IHCDescriptorPool> globalDescriptorPool{};
        std::unique_ptr<IHCDescriptorPool> localDescriptorPool{}; 

        // DescriptorSetLayout: slots on the pipeline
        // accepting descriptor sets to be bound
        std::unique_ptr <IHCDescriptorSetLayout> globalDescriptorSetLayout; // UNIFORM_BUFFER
        std::unique_ptr <IHCDescriptorSetLayout> textureDescriptorSetLayout; // COMBINED_IMAGE_SAMPLER
        std::unique_ptr <IHCDescriptorSetLayout> skeletalDescriptorSetLayout; // UNIFORM_BUFFER

        // resource having camera matrices, global light info
        std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> globalUBOs;
        // resource having textures are in assetManager texture repo

        //// Camera matrices
        // pointer to resource: globalUBOs
        std::vector<VkDescriptorSet> globalDescriptorSets;

        //// Texture
        // pointer to resource: textures
        std::vector<VkDescriptorSet> textureDescriptorSets;
        // Stack of available descriptor sets, texture load/ unload
        std::stack<VkDescriptorSet> availableTextureDescriptorSets; 

        //// Skeletal
        std::vector<std::unique_ptr<IHCBuffer>> skeletalUBOs;
        std::vector<VkDescriptorSet> skeletalDescriptorSets;
        std::stack<IHCBuffer*> availableSkeletalUBOs;
        std::stack<VkDescriptorSet> availableSkeletalDescriptorSets;
        std::unique_ptr<IHCBuffer> dummySkeletalUBO; // If animation not used, we still need one for validation errors
        VkDescriptorSet dummySkeletalDescriptorSet;
	};
}


// Analogy
// Buffers: (store ingredients)
// DescriptorSetLayout: (recipe, how to assemble the ingredients)
// DescriptorPool: Preparation table with limited space (memory). Dishes (DescriptorSets) occupy this space
// DescriptorSets: Dishes on the preparation table

