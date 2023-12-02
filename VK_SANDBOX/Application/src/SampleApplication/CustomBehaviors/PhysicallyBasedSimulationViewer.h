#pragma once

#include "../../../../Engine/src/Physics/SoftBody.h"
#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"

namespace IHCEngine::Component
{
	class MeshComponent;
}

namespace SampleApplication
{
    class PhysicallyBasedSimulationViewer : public IHCEngine::Component::CustomBehavior
    {
    public:
        PhysicallyBasedSimulationViewer() = default;
        ~PhysicallyBasedSimulationViewer() = default;

        void Awake() override;
        void Start() override;
        void Update() override;
        void FixedUpdate() override;
        void OnEnable() override;
        void OnDisable() override;


    private:

        // Wait for frame rate to stable after scene change
        float startIntegrateTime = 3.0;
        float accumulatedTime = 0;

        // Cloth
        IHCEngine::Physics::SoftBody cloth;

        //Sphere
        IHCEngine::Core::GameObject* sphereGobj = nullptr;
        float movementSpeed = 5;
        void sphereControl();




    };
}
