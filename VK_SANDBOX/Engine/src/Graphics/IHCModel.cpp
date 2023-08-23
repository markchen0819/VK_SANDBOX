#include "../pch.h"
#include "IHCModel.h"

#pragma region tiny_obj_loader loadModel function
void IHCEngine::Graphics::IHCModel::Builder::LoadModel(const std::string& filepath)
{
    tinyobj::attrib_t attrib; // positions, normals and texture coordinates
    std::vector<tinyobj::shape_t> shapes; //separate objects and their faces
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    vertices.clear();
    indices.clear();

    // combine all of the faces in the file into a single model,
    // triangulation feature has already made sure that there are three vertices per face
    std::unordered_map<Vertex, uint32_t> uniqueVertices{}; // avoid duplicate vertex

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};

            if (index.vertex_index >= 0) 
            {
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                };

                vertex.color = {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2],
                };

            }

            if (index.normal_index >= 0)
            {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                };
            }

            if (index.texcoord_index >= 0)
            {
                vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1],
                };

                //vertex.uv = {
                //{
                //    attrib.texcoords[2 * index.texcoord_index + 0],
                //    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                //};
            }

            if (uniqueVertices.count(vertex) == 0) 
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);

        }
    }
}
#pragma endregion

std::unique_ptr<IHCEngine::Graphics::IHCModel> IHCEngine::Graphics::IHCModel::createModelFromFile(IHCDevice& device, const std::string& filepath)
{
    Builder builder{};
    builder.LoadModel(filepath);
    std::cout << "Vertex Count" << builder.vertices.size() << std::endl;
    return std::make_unique<IHCEngine::Graphics::IHCModel>(device, builder);
}

IHCEngine::Graphics::IHCModel::IHCModel(IHCDevice& device, const IHCModel::Builder& builder) 
    : ihcDevice{ device }
{
	createVertexBuffers(builder.vertices);
	createIndexBuffers(builder.indices);
}

IHCEngine::Graphics::IHCModel::~IHCModel()
{
	// destory buffer free memory
}


#pragma region Vertex/Index buffer creation for drawing
// staging buffer great for static data (model with same vertex/index data)
void IHCEngine::Graphics::IHCModel::createVertexBuffers(const std::vector<Vertex>& vertices)
{
    vertexCount = static_cast<uint32_t>(vertices.size());
    assert(vertexCount >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
    uint32_t vertexSize = sizeof(vertices[0]);

    // temporary buffer accessed by CPU and GPU
    LveBuffer stagingBuffer{
        ihcDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };
    // without HOST_VISIBLE, HOST_COHERENT we'll need vkflush

    // map temporary buffer memory to CPU address space, Copy vertex data to staging buffer
    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)vertices.data());

    // actual vertex buffer, staging buffer to the vertex buffer by the GPU
    vertexBuffer = std::make_unique<LveBuffer>(
        lveDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    ihcDevice.CopyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}
void IHCEngine::Graphics::IHCModel::createIndexBuffers(const std::vector<uint32_t>& indices)
{
    // Check if using index buffer
    indexCount = static_cast<uint32_t>(indices.size());
    hasIndexBuffer = indexCount > 0;
    if (!hasIndexBuffer)
    {
        return; // No more work
    }
    VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
    uint32_t indexSize = sizeof(indices[0]);

    // temporary buffer accessed by CPU and GPU
    LveBuffer stagingBuffer{
        lveDevice,
        indexSize,
        indexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };
    // without HOST_VISIBLE, HOST_COHERENT we'll need vkflush

   // map temporary buffer memory to CPU address space, Copy index data to staging buffer
    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void*)indices.data());

    // actual index buffer, staging buffer to the index  buffer by the GPU
    indexBuffer = std::make_unique<LveBuffer>(
        lveDevice,
        indexSize,
        indexCount,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT
        | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    lveDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
}
#pragma endregion



#pragma region Draw
void IHCEngine::Graphics::IHCModel::Bind(VkCommandBuffer commandBuffer)
{
    VkBuffer buffers[] = { vertexBuffer->getBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    if (hasIndexBuffer) 
    {
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
}
void IHCEngine::Graphics::IHCModel::Draw(VkCommandBuffer commandBuffer)
{
    if (hasIndexBuffer) 
    {
        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
    }
    else 
    {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }
}
#pragma endregion


