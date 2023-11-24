#pragma once

#include "../../../../Engine/src/Physics/PhysicsSolver.h"
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
        PhysicallyBasedSimulationViewer();
        ~PhysicallyBasedSimulationViewer();

        void Awake() override;
        void Start() override;
        void Update() override;
        void FixedUpdate() override;
        void OnEnable() override;
        void OnDisable() override;


    private:

        IHCEngine::Physics::PhysicsSolver physicsSolver;
    };
}
