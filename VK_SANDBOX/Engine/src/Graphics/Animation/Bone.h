#pragma once
#include <assimp/anim.h>

// A single bone
// reads all keyframes data from aiNodeAnim
// interpolate between its keys (TRS) based on the current animation time.

namespace IHCEngine::Graphics
{
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


	class Bone
	{
	public:

        Bone(const std::string& name, int ID, const aiNodeAnim* channel);
        void Update(float animationTime);
        glm::mat4 GetLocalTransform() const { return localTransform; }
        std::string GetBoneName() const { return name; }
        int GetBoneID() const { return id; }

        // Current Index to interpolate to based on the current animation time
        int GetPositionIndex(float animationTime);
        int GetRotationIndex(float animationTime);
        int GetScaleIndex(float animationTime);

    private:

        // Gets normalized value for Lerp & Slerp used for interpolation
        float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
        glm::mat4 interpolatePosition(float animationTime);
        glm::mat4 interpolateRotation(float animationTime);
        glm::mat4 interpolateScaling(float animationTime);

        std::vector<KeyPosition> keyPositions;
        std::vector<KeyRotation> keyRotations;
        std::vector<KeyScale> keyScales;
        int numPositions;
        int numRotations;
        int numScales;

        glm::mat4 localTransform;
        std::string name;
        int id;
	};

}