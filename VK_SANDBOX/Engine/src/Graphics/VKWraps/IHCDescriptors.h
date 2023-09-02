#pragma once
// Forward declaration
namespace IHCEngine::Graphics
{
    class IHCDevice;
}

namespace IHCEngine::Graphics
{
	// Descriptor example:
    // 
    // uniform, storage, image buffers and etc cannot directly communicate to pipeline
    // -> buffer Descriptor, image descriptor (points to resource)
    // -> must group as SET to communicate (performance, can BIND) 
    //
    // Descriptor Set Layouts needed to be provided to pipeline (blueprint)
    // 
    // (Global, bind once and reuse in drawcall)
    // Descriptor Set 0 - proj/view matrix ubo (likely be the same for every object) 
    //                  - point light ubo
    // (Resources change per object, bind before each draw)
    // Descriptor Set 1 - texture 0
    //                  - texture 1
    //                  - texture 2 
    //                  - texture 3
    // Descriptor Set 2 - model matrix
    //
    // Code layout
    // bind pipeline
    // bind global_descriptor_set (0)
    // for each material
    //     bind material_descriptor_set (1)
    //     for each object using the material
    //           bind object_descriptor_set (2)
    //           bind object model
    //           draw


    // The DescriptorSetLayout describes the types of resources 
    // that will be accessed by the pipeline stages, and 
    // in what format and order.
    class IHCDescriptorSetLayout 
    {
    public:

        // Helper to build DescriptorSetLayout 
        class Builder
        {
        public:
            Builder(IHCDevice& ihcDevice) : ihcDevice{ ihcDevice } {}
            // reference chaining
            Builder& AddBinding 
            (
                // The binding point that this binding will be used in the shader
                // GLSL: layout(binding = 0) uniform XXX 
                uint32_t binding, 
                VkDescriptorType descriptorType, // uniform, storage, image buffer
                VkShaderStageFlags stageFlags, // which stages had access
                uint32_t count = 1
            );
            // Build DescriptorSetLayouts (multiple) to provide to pipeline
            std::unique_ptr<IHCDescriptorSetLayout> Build() const;

        private:
            IHCDevice& ihcDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        // Ctor Dtor
        IHCDescriptorSetLayout(IHCDevice& ihcDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~IHCDescriptorSetLayout();

        // no duplication
        IHCDescriptorSetLayout(const IHCDescriptorSetLayout&) = delete;
        IHCDescriptorSetLayout& operator=(const IHCDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout GetDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        IHCDevice& ihcDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class IHCDescriptorWriter;
    };

    // This is where we allocate descriptor sets, which are used to
    // pass resources like buffers and images to the shaders.
    class IHCDescriptorPool
    {
    public:
        // Helper to create DescriptorPool 
        class Builder
        {
        public:
            Builder(IHCDevice& ihcDevice) : ihcDevice{ ihcDevice } {}

            // reference chaining
            Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& SetMaxSets(uint32_t count);
            std::unique_ptr<IHCDescriptorPool> Build() const;

        private:
            IHCDevice& ihcDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        // Ctor Dtor
        IHCDescriptorPool(IHCDevice& ihcDevice,uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~IHCDescriptorPool();

        // no duplication
        IHCDescriptorPool(const IHCDescriptorPool&) = delete;
        IHCDescriptorPool& operator=(const IHCDescriptorPool&) = delete;

        bool AllocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
        void FreeDescriptorSet(std::vector<VkDescriptorSet>& descriptors) const;
        void ResetPool();

    private:
        IHCDevice& ihcDevice;
        VkDescriptorPool descriptorPool;

        friend class IHCDescriptorWriter;
    };

    // Allocate VkDescriptorSet from pool
    // and write necessary info for each Descriptor in the set
    // use friend class to access private members
    class IHCDescriptorWriter 
    {
    public:
        IHCDescriptorWriter(IHCDescriptorSetLayout& setLayout, IHCDescriptorPool& pool);

        // specify which resources (buffers or images) should be bound to which shader binding points
        IHCDescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        IHCDescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool Build(VkDescriptorSet& set); // allocate a new descriptor set
        void Overwrite(VkDescriptorSet& set); //  update with the specified data

    private:
        IHCDescriptorSetLayout& setLayout;
        IHCDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes; // commands to bind a buffer, bind an image, etc
    };
}
