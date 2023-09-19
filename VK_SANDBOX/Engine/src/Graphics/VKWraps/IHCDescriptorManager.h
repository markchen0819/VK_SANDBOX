#pragma once
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

        // Get Resources tp write into and store
        std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>>& GetGlobalUBOs() { return globalUBOs; }

        // Get DescriptorSets to bind to pipeline slot for access (set up shader binding)
        std::vector<VkDescriptorSet> GetGlobalDescriptorSets() { return globalDescriptorSets; }
        std::unordered_map<std::string, std::vector<VkDescriptorSet>>& GetTextureToDescriptorSetsMap() { return textureToDescriptorSetsMap; }

        void AllocateTextureDescriptorSetForTexture(IHCTexture* texture);
        void DeallocateTextureDescriptorSetForTexture(std::string assetName);

        void AllocateSkeletalDescriptorSetForAnimator(Animator* animator);
        void DeallocateSkeletalDescriptorSetForAnimator(Animator* animator);

        IHCEngine::Graphics::IHCBuffer* GetSkeletalUBOByIndex(int index) { return skeletalUBOs[index].get(); }
        std::unordered_map<Animator*, std::vector<VkDescriptorSet>>& GetAnimatorToDescriptorSetsMap() { return animatorToDescriptorSetsMap; }
        std::unordered_map<Animator*, std::vector<int>>& GetAnimatorToSkeletalUBOIndexMap() { return animatorToSkeletalUBOIndexMap; }
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
        // Use map to track texture <-> descriptorset
        // holds the info per scene (as tecture is loaded/ unloaded per scene)
        std::unordered_map<std::string, std::vector<VkDescriptorSet>> textureToDescriptorSetsMap;

        //// Skeletal
        std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> skeletalUBOs;
        int skeletalUBOIndex = 0;
        std::stack<VkDescriptorSet> availableSkeletalDescriptorSets;
        std::vector<VkDescriptorSet> skeletalDescriptorSets;
        std::unordered_map<Animator*, std::vector<VkDescriptorSet>> animatorToDescriptorSetsMap;
        std::unordered_map<Animator*, std::vector<int>> animatorToSkeletalUBOIndexMap;
	};
}


// Analogy
// Buffers: (store ingredients)
// DescriptorSetLayout: (recipe, how to assemble the ingredients)
// DescriptorPool: Preparation table with limited space (memory). Dishes (DescriptorSets) occupy this space
// DescriptorSets: Dishes on the preparation table

