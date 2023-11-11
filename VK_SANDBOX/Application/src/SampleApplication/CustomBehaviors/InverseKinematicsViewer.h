#pragma once

#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"

namespace IHCEngine::Component
{
	class LineRendererComponent;
}

namespace IHCEngine::Graphics
{
	struct SkeletalNodeData;
}

namespace IHCEngine::Component
{
	class AnimatorComponent;
    class IKComponent;
}


namespace SampleApplication
{
    class InverseKinematicsViewer : public IHCEngine::Component::CustomBehavior
    {
    public:
        InverseKinematicsViewer();
        ~InverseKinematicsViewer();

        void Awake() override;
        void Start() override;
        void Update() override;
        void FixedUpdate() override;
        void OnEnable() override;
        void OnDisable() override;

        // Imgui
        void SetMode(std::string m);
        bool IsAnimating() { return !trigger; }

    private:


        bool trigger = true;
        std::string mode = "IK_Animation";

        // Target Gobj
        IHCEngine::Core::GameObject* movingGobj = nullptr;

    	// IKGobj
        IHCEngine::Core::GameObject* IKGobj = nullptr;
        IHCEngine::Component::IKComponent* ikComponent = nullptr;
        IHCEngine::Component::AnimatorComponent* animatorComponent = nullptr;
        float movementSpeed = 10;

        // Path
        IHCEngine::Component::LineRendererComponent* lineRenderer;
        std::vector<IHCEngine::Core::GameObject*> debugControlPoints;

        // keyframes
        glm::vec3 eePositionAtKeyframeStart;
        float currentTime = 0;
        float totalTime = 1.5; //4 seconds

        void TargetObjectInputs();
        void IK_Animation();
        void IK_RealTime();

    };
}
