#pragma once

#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"
#include "../../../../Engine/src/Math/Path/SpaceCurve.h"
#include "../../../../Engine/src/Math/Path/SpeedControl.h"
#include "../../../../Engine/src/Math/Path/PaceControl.h"
#include "../../../../Engine/src/Math/Path/OrientationControl.h"

namespace IHCEngine::Component
{
	class LineRendererComponent;
}

namespace IHCEngine::Graphics
{
	struct SkeletalNodeData;
    class Camera;
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

        // keyframes
        glm::vec3 eePositionAtKeyframeStart;
        float currentTime = 0;
        float totalAnimationTime = 1.5; // seconds

        void TargetObjectInputs();
        void IK_Animation();
        void IK_RealTime();

        // Path
        IHCEngine::Component::LineRendererComponent* lineRenderer;
        IHCEngine::Math::SpaceCurve spaceCurve;
        std::vector<glm::vec3> data;
        std::vector<IHCEngine::Core::GameObject*> debugControlPoints;
        int controlPointID = 0;
        void createDebugControlPoints();
        // Speed
        IHCEngine::Math::SpeedControl speedControl;
        IHCEngine::Math::PaceControl paceControl;
        IHCEngine::Math::OrientationControl orientationControl;
        float ikGameObjectMovementSpeed = 7.0;
        float passedTime = 0;
        float easeInTiming = 0;
        float easeOutTiming = 0;
        float totalWalkTime = 10;
        float currentSpeed = 0;
        float prevFrameDistance = 0;

        // AnimationViewerInput
        IHCEngine::Graphics::Camera* camera = nullptr;
        glm::vec3 centerPoint = glm::vec3(0.0);
        float angleRespectToCenterPoint;
        float distanceToCenterPoint;
        float angleSpeed = 50.0;
        float movementSpeed = 10;
        float zoomSpeed = 0.03f;
        void HandleAnimationViewerInput();

    };
}
