#pragma once


namespace IHCEngine::Graphics
{
    class InverseKinematicsSolver
    {
    public:

        void SetJoints(const std::vector<glm::vec3>& initialJoints);

        void Solve_FABRIK(glm::vec3 target);

        // Create IK component
        // void Animator::calculateBoneTransformVQS(const SkeletalNodeData* node, Math::VQS parentVQS)
    private:

        int MAX_ITERATIONS = 10;
        int USER_EPSILON = 0.1;
        std::vector<glm::vec3> joints;
        std::vector<float> distances;
        float totalDistance = 0;
    };
}
