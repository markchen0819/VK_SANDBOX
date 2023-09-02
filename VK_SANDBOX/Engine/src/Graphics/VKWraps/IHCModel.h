#pragma once

// Forward declaration
struct Vertex;  
namespace IHCEngine::Graphics
{
    class IHCDevice;
    class IHCBuffer;
}

namespace IHCEngine::Graphics
{
	class IHCModel
	{
    public:

        struct Builder
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
            void LoadModel(const std::string& filepath);
        };

        IHCModel(IHCDevice& device, const IHCModel::Builder& builder);
        ~IHCModel();

        // no duplication
        IHCModel(const IHCModel&) = delete;
        IHCModel &operator=(const IHCModel&) = delete;

        static std::unique_ptr<IHCModel> CreateModelFromFile
        (IHCDevice& device, const std::string& filepath);

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);

    private:

        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices);

        IHCDevice& ihcDevice;

        std::unique_ptr<IHCBuffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        std::unique_ptr<IHCBuffer> indexBuffer;
        uint32_t indexCount;
    };
}
