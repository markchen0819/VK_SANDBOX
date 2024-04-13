#pragma once

#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"

namespace IHCEngine::Component
{
	class ComputeFluidComponent;
}

namespace IHCEngine::Graphics
{
	class Camera;
}

namespace SampleApplication
{
    class FluidInteractionSphere : public IHCEngine::Component::CustomBehavior
    {
    public:
        void Awake() override;
        void Update() override;

        void SetComputeFluid(IHCEngine::Component::ComputeFluidComponent* c) { computeFluid = c; }

    private:
        IHCEngine::Graphics::Camera* camera = nullptr;
        IHCEngine::Component::ComputeFluidComponent* computeFluid = nullptr;
        float movementSpeed = 30.0f;
    };
}
