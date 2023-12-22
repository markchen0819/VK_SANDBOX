#pragma once
namespace IHCEngine::Graphics
{
	class IHCTexture;

    // Parsed data from Assimp
	struct MaterialData
    {
        std::string name;
        glm::vec3 diffuse {glm::vec3(1.0)};
        glm::vec3 specular {glm::vec3(0.0)};;
        float shininess = 0.1f;
        std::vector<IHCTexture*> diffuseMaps;
        std::vector<IHCTexture*> specularMaps;
        std::vector<IHCTexture*> normalMaps;
        std::vector<IHCTexture*> heightMaps;
    };
}