#pragma once

#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"

namespace IHCEngine::Component
{
	class AnimatorComponent;
}

namespace IHCEngine::Component
{
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


    private:

        // Target Gobj
        IHCEngine::Core::GameObject* movingGobj = nullptr;
        // IKGobj
        IHCEngine::Core::GameObject* IKGobj = nullptr;
        IHCEngine::Component::IKComponent* ikComponent = nullptr;
        IHCEngine::Component::AnimatorComponent* animatorComponent = nullptr;
        float movementSpeed = 10;

        bool trigger = false;

        void TargetObjectInputs();

    };
}
