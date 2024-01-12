#pragma once
namespace IHCEngine::Graphics
{
    struct Particle
	{
        glm::vec4 position;
        glm::vec4 rotation;
        glm::vec4 scale;
        glm::vec4 velocity;
        glm::vec4 color;



        // Below is used for Vertex input
        // stop using this since we change from rendering points into mesh

     //   static VkVertexInputBindingDescription getBindingDescription()
    	//{
     //       VkVertexInputBindingDescription bindingDescription{};
     //       bindingDescription.binding = 0;
     //       bindingDescription.stride = sizeof(Particle);
     //       bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
     //       // instanced rendering: VK_VERTEX_INPUT_RATE_INSTANCE

     //       return bindingDescription;
     //   }

     //   static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
    	//{
     //       std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

     //       attributeDescriptions[0].binding = 0;
     //       attributeDescriptions[0].location = 0;
     //       attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
     //       attributeDescriptions[0].offset = offsetof(Particle, position);

     //       attributeDescriptions[1].binding = 0;
     //       attributeDescriptions[1].location = 1;
     //       attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
     //       attributeDescriptions[1].offset = offsetof(Particle, color);

     //       return attributeDescriptions;
     //   }
    };
}