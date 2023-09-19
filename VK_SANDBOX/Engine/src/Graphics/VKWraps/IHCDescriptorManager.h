#pragma once
namespace IHCEngine::Graphics
{
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

        void AllocateLocalDescriptorSetForTexture(IHCTexture* texture);
        void DeallocateLocalDescriptorSetForTexture(std::string assetName);

	private:
        void createDescriptorSetLayouts();
        void initPool();
        void createGlobalUniformBuffers();
        void allocateGlobalDescriptorSets();
       

        int TEXTURE_COUNT_LIMIT = 200;

        IHCDevice& ihcDevice;
        std::unique_ptr<IHCDescriptorPool> globalDescriptorPool{};
        std::unique_ptr<IHCDescriptorPool> localDescriptorPool{}; 

        // DescriptorSetLayout: slots on the pipeline
        // accepting descriptor sets to be bound
        std::unique_ptr <IHCDescriptorSetLayout> globalDescriptorSetLayout; // UNIFORM_BUFFER
        std::unique_ptr <IHCDescriptorSetLayout> localDescriptorSetLayout; // COMBINED_IMAGE_SAMPLER


        // resource having camera matrices, global light info
        std::vector<std::unique_ptr<IHCEngine::Graphics::IHCBuffer>> globalUBOs;
        // resource having textures are in assetManager texture repo


        // pointer to resource: globalUBOs
        std::vector<VkDescriptorSet> globalDescriptorSets;

        // pointer to resource: textures
        std::vector<VkDescriptorSet> localDescriptorSets;
        // Stack of available descriptor sets, texture load/ unload
        std::stack<VkDescriptorSet> availableLocalDescriptorSets; 

        // Use map to track texture <-> descriptorset
        // holds the info per scene (as tecture is loaded/ unloaded per scene)
        std::unordered_map<std::string, std::vector<VkDescriptorSet>> textureToDescriptorSetsMap;

	};
}


// Analogy
// Buffers: (store ingredients)
// DescriptorSetLayout: (recipe, how to assemble the ingredients)
// DescriptorPool: Preparation table with limited space (memory). Dishes (DescriptorSets) occupy this space
// DescriptorSets: Dishes on the preparation table

