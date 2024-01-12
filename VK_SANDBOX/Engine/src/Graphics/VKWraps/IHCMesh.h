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
	class IHCMesh
	{
    public:

        struct Builder
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
            void LoadMesh(const std::string& filepath);
        };

        IHCMesh(IHCDevice& device, const IHCMesh::Builder& builder);
        ~IHCMesh();

        // no duplication
        IHCMesh(const IHCMesh&) = delete;
        IHCMesh &operator=(const IHCMesh&) = delete;

        static std::unique_ptr<IHCMesh> CreateMeshFromFile
        (IHCDevice& device, const std::string& filepath);

        // data are written into buffer when created
        // but in Model extra vertex data is apply when bone is used;
        void UpdateVertices();
		
        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);
        void InstanceDraw(VkCommandBuffer commandBuffer, int instanceCount); //GPU instancing

        std::vector<Vertex>& GetVertices() { return vertices; } // for bones

    private:

        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices);

        IHCDevice& ihcDevice;

        std::unique_ptr<IHCBuffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        std::unique_ptr<IHCBuffer> indexBuffer;
        uint32_t indexCount;

        std::vector<Vertex> vertices; // for bones
    };
}
