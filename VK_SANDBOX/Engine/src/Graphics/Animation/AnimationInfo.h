#pragma once

namespace IHCEngine::Graphics
{
    // Model node hiearachy
    struct AssimpNodeData
    {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };

    // Inverse bind pose
    struct BoneInfo
    {
        int id; // index in finalBoneMatrices
        glm::mat4 offsetMatrix;
    	// offset matrix transforms vertex from
    	// model space to bone space
    };

    struct KeyPosition
    {
        glm::vec3 position;
        float timeStamp;
    };
    struct KeyRotation
    {
        glm::quat orientation;
        float timeStamp;
    };
    struct KeyScale
    {
        glm::vec3 scale;
        float timeStamp;
    };

}

// Reference
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation