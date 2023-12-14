#pragma once

#include "../../../../Engine/src/Physics/SoftBody.h"
#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"

namespace IHCEngine::Graphics
{
	class Camera;
}

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

        // Imgui
        void SetPinned(int pinID);
        bool IsP1Pinned() const { return isP1Pinned; }
        bool IsP2Pinned() const { return isP2Pinned; }
        bool IsP3Pinned() const { return isP3Pinned; }
        bool IsP4Pinned() const { return isP4Pinned; }
        bool HasStartedSimulating() { return (accumulatedTime > startIntegrateTime); }
        void SetWindStrength(float f) { windStrength = f; }
        IHCEngine::Physics::SoftBody& GetCloth() { return cloth; }

    private:

        // Wait for frame rate to stable after scene change
        float startIntegrateTime = 3.0;
        float accumulatedTime = 0;

        // Cloth
        IHCEngine::Physics::SoftBody cloth;
        glm::vec3 gravityAcceleration = glm::vec3(0.0f, -9.81, 0.0f);
        glm::vec3 windForce = glm::vec3(0);
        float windStrength = 0.0f;

        //Sphere
        IHCEngine::Core::GameObject* sphereGobj = nullptr;
        float sphereMovementSpeed = 5;
        void sphereControl();

        // AnimationViewerInput
        IHCEngine::Graphics::Camera* camera = nullptr;
        glm::vec3 centerPoint = glm::vec3(0.0);
        float angleRespectToCenterPoint;
        float distanceToCenterPoint;
        float angleSpeed = 50.0;
        float movementSpeed = 10;
        float zoomSpeed = 0.03f;
        void HandleAnimationViewerInput();

        // Imgui
        bool isP1Pinned = true;
        bool isP2Pinned = true;
        bool isP3Pinned = true;
        bool isP4Pinned = true;
    };
}
