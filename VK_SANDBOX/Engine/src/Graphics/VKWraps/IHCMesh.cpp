#include "../../pch.h"
#include "IHCMesh.h"

#include <tiny_obj_loader.h>
#include "VKHelpers.h"
#include "IHCDevice.h"
#include "IHCBuffer.h"

#pragma region tiny_obj_loader loadModel function
void IHCEngine::Graphics::IHCMesh::Builder::LoadMesh(const std::string& filepath)
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

	std::vector<Vertex> tempVertices;
    std::vector<uint32_t> tempIndices;

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

                // using the vk tutorial model
                //vertex.uv = {
                //    attrib.texcoords[2 * index.texcoord_index + 0],
                //    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                //};
            }

            if (uniqueVertices.count(vertex) == 0) 
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(tempVertices.size());
                tempVertices.push_back(vertex);
            }
            tempIndices.push_back(uniqueVertices[vertex]);

        }
    }


    // https://learnopengl.com/Advanced-Lighting/Normal-Mapping

    std::vector<glm::vec3> tempTangents(tempVertices.size(), glm::vec3(0.0f));
    std::vector<glm::vec3> tempBitangents(tempVertices.size(), glm::vec3(0.0f));

    for (size_t i = 0; i < tempIndices.size(); i += 3)
    {
        auto& v0 = tempVertices[tempIndices[i]];
        auto& v1 = tempVertices[tempIndices[i + 1]];
        auto& v2 = tempVertices[tempIndices[i + 2]];

        glm::vec3 edge1 = v1.position - v0.position;
        glm::vec3 edge2 = v2.position - v0.position;
        glm::vec2 deltaUV1 = v1.uv - v0.uv;
        glm::vec2 deltaUV2 = v2.uv - v0.uv;

        glm::vec3 tangent;
        glm::vec3 bitangent;

        float denom = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
        if (fabs(denom) < 1e-6) 
        {
            // Handle degenerate UVs
            tangent = glm::vec3(1.0, 0.0, 0.0);
            bitangent = glm::vec3(0.0, 1.0, 0.0);
        }
        else 
        {
            float f = 1.0f / denom;
            tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
            bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);
        }

        tempTangents[tempIndices[i]] += tangent;
        tempTangents[tempIndices[i + 1]] += tangent;
        tempTangents[tempIndices[i + 2]] += tangent;

        tempBitangents[tempIndices[i]] += bitangent;
        tempBitangents[tempIndices[i + 1]] += bitangent;
        tempBitangents[tempIndices[i + 2]] += bitangent;
    }

    for (size_t i = 0; i < tempVertices.size(); ++i)
    {
        tempVertices[i].tangent = glm::normalize(tempTangents[i]);
        tempVertices[i].bitangent = glm::normalize(tempBitangents[i]);
    }

    vertices = std::move(tempVertices);
    indices = std::move(tempIndices);

}
#pragma endregion

std::unique_ptr<IHCEngine::Graphics::IHCMesh> IHCEngine::Graphics::IHCMesh::CreateMeshFromFile(IHCDevice& device, const std::string& filepath)
{
    Builder builder{};
    builder.LoadMesh(filepath);
    std::cout << "Vertex Count" << builder.vertices.size() << std::endl;
    return std::make_unique<IHCEngine::Graphics::IHCMesh>(device, builder);
}

void IHCEngine::Graphics::IHCMesh::UpdateVertices()
{
    vkDeviceWaitIdle(ihcDevice.GetDevice());

    // old
	//createVertexBuffers(this->GetVertices());

    // new
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
    uint32_t vertexSize = sizeof(vertices[0]);
    IHCBuffer stagingBuffer
    {
        ihcDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };
    stagingBuffer.Map();
    stagingBuffer.WriteToBuffer((void*)vertices.data());
    stagingBuffer.Unmap(); //also handled automatically in destructor
    ihcDevice.CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);
}

IHCEngine::Graphics::IHCMesh::IHCMesh(IHCDevice& device, const IHCMesh::Builder& builder) 
    : ihcDevice{ device }
{
	createVertexBuffers(builder.vertices);
	createIndexBuffers(builder.indices);
}

IHCEngine::Graphics::IHCMesh::~IHCMesh()
{
    vkDeviceWaitIdle(ihcDevice.GetDevice());
	// destory buffer free memory
}


#pragma region Vertex/Index buffer creation for drawing
// staging buffer great for static data (model with same vertex/index data)
void IHCEngine::Graphics::IHCMesh::createVertexBuffers(const std::vector<Vertex>& vertices)
{
    this->vertices = vertices;
    vertexCount = static_cast<uint32_t>(vertices.size());
    assert(vertexCount >= 3 && "Vertex count must be at least 3");

    // temporary buffer accessed by CPU and GPU
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
    uint32_t vertexSize = sizeof(vertices[0]);
    IHCBuffer stagingBuffer
    {
        ihcDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };
    // without HOST_VISIBLE, HOST_COHERENT we'll need vkflush

    // map temporary buffer memory to CPU address space
    // (able to write/read on CPU )
    // Copy vertex data to staging bufferF
    stagingBuffer.Map();
    stagingBuffer.WriteToBuffer((void*)vertices.data());
    stagingBuffer.Unmap(); //also handled automatically in destructor

    // actual vertex buffer, staging buffer to the vertex buffer by the GPU
    vertexBuffer = std::make_unique<IHCBuffer>(
        ihcDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    ihcDevice.CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);
}
void IHCEngine::Graphics::IHCMesh::createIndexBuffers(const std::vector<uint32_t>& indices)
{
    // Check if using index buffer
    indexCount = static_cast<uint32_t>(indices.size());
    hasIndexBuffer = indexCount > 0;
    if (!hasIndexBuffer)
    {
        return; // No more work
    }

    // temporary buffer accessed by CPU and GPU
    VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
    uint32_t indexSize = sizeof(indices[0]);
    IHCBuffer stagingBuffer{
        ihcDevice,
        indexSize,
        indexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };
    // without HOST_VISIBLE, HOST_COHERENT we'll need vkflush

    // map temporary buffer memory to CPU address space, Copy index data to staging buffer
    stagingBuffer.Map();
    stagingBuffer.WriteToBuffer((void*)indices.data());
    stagingBuffer.Unmap(); //also handled automatically in destructor

    // actual index buffer, staging buffer to the index buffer by the GPU
    indexBuffer = std::make_unique<IHCBuffer>(
        ihcDevice,
        indexSize,
        indexCount,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT
        | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    ihcDevice.CopyBuffer(stagingBuffer.GetBuffer(), indexBuffer->GetBuffer(), bufferSize);
}
#pragma endregion



#pragma region Draw
void IHCEngine::Graphics::IHCMesh::Bind(VkCommandBuffer commandBuffer)
{
    VkBuffer buffers[] = { vertexBuffer->GetBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    if (hasIndexBuffer) 
    {
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
}
void IHCEngine::Graphics::IHCMesh::Draw(VkCommandBuffer commandBuffer)
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

void IHCEngine::Graphics::IHCMesh::InstanceDraw(VkCommandBuffer commandBuffer, int instanceCount)
{
    if (hasIndexBuffer)
    {
        vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, 0, 0, 0);
    }
    else
    {
        vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, 0);
    }
}
#pragma endregion


