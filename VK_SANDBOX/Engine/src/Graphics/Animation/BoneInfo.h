#pragma once
struct BoneInfo
{
    int id; // index in finalBoneMatrices
    glm::mat4 offsetMatrix; // offset matrix transforms vertex from model space to bone space
};

// Reference
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation