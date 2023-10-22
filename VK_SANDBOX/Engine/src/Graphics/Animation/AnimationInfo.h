#pragma once
#include "../../Math/VQS.h"

namespace IHCEngine::Graphics
{
    // Logic:
    //
    // 1. Use SkeletalNodeData with hierarchy to concat
    //    finalBoneTransforms (as one bone affects another)
    // 2. finalBoneTransforms applied offsetMatrix
    //    so that not only bones are moved
    //    but vertices related to the bones are moved
    // 3. boneWeights and boneIDs are stored on a vertex
    //    to know how the bone influence the vertex position
    //    and get final vertex position


    // Relation Of Space
    //
    // SkeletalNodeData : bone's local space relative to parent bone's space
    // Concat SkeletalNodeData up to root -> Model Space
    // SkinningInfo:  Model Space -> target bone's local space to apply influence
    //
    // Order: bone movement updates
    // -> calculate global transformations for bones
    // -> skinning process (vertex deformation)


    // Represents each bone in the skeleton hierarchy of the model
    // holds the transformation matrix for the bone
    // from bone's local space to  parent bone's space within the hierarchy
    struct SkeletalNodeData
    {
        std::string name;

        SkeletalNodeData* parent = nullptr;
        std::vector<SkeletalNodeData> children;
        int childrenCount;

        // Using Matrix
        glm::mat4 transformation_Matrix;
  
		// Using VQS
        IHCEngine::Math::VQS transformation_VQS;
    };

    // Represents Inverse bind pose
    // (Skinning, deform model vertices based on the bone)
    // holds the offsetMatrix for a vertex
    // from initial model space to bone local space
    struct SkinningInfo
    {
        int id;
        glm::mat4 offsetMatrix;

    	// id:
    	//     index in finalBoneMatrices
        //     which is the boneMatrix that affects this vertex
       
        // offsetMatrix:
        //     transform vertex from T-pose
        //      -> move to origin
        //      -> move to position after bone moved (bone's local space)

        // BoneWeights are stored in vertex
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