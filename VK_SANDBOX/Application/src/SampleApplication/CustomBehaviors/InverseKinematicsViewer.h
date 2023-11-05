#pragma once

#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"

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
        float movementSpeed = 10;
        void TargetObjectInputs();

    };
}
