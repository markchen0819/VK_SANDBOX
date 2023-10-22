#pragma once
#include <GLFW/glfw3.h>
#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"
#include "../../../../Engine/src/Math/Path/SpaceCurve.h"
#include "../../../../Engine/src/Math/Path/SpeedControl.h"
#include "../../../../Engine/src/Math/Path/OrientationControl.h"
#include "../../../../Engine/src/Math/Path/PaceControl.h"
#include "../../../../Engine/src/Graphics/Animation/BlendTree.h"

namespace IHCEngine::Component
{
	class AnimatorComponent;
	class LineRendererComponent;
}

namespace IHCEngine::Graphics
{
	class IHCMesh;
	class Camera;
}

namespace SampleApplication
{
    class MotionAlongPathViewer : public IHCEngine::Component::CustomBehavior
    {
    public:
        MotionAlongPathViewer();
        ~MotionAlongPathViewer();

        void Awake() override;
        void Start() override;
        void Update() override;
        void FixedUpdate() override;
        void OnEnable() override;
        void OnDisable() override;

        // Imgui
        std::vector<glm::vec3>& GetControlPoints() { return data; }
        ArcLengthTable& GetArcLengthTable();
        float GetCurrentSpeed() { return currentSpeed; }
        float GetTimePassed() { return passedTime; }

        float GetEaseInTiming() { return easeInTiming; }
        float GetEaseOutTiming() { return easeOutTiming; }
        float GetTotalTime() { return totalTime; }

        float GetBlendRatio() { return blendRatio; }
        float GetUpperBoundSpeed() { return upperBoundSpeed; }
        float GetLowerBoundSpeed() { return lowerBoundSpeed; }

    private:

        bool isMoving = false;
        float currentSpeed = 0;
        float prevFrameDistance = 0;
        float passedTime = 0;

        // Viewer
        GLFWwindow* window = nullptr;
        IHCEngine::Graphics::Camera* camera = nullptr;
        glm::vec3 centerPoint = glm::vec3(0.0);
        float angleRespectToCenterPoint;
        float distanceToCenterPoint;
        float angleSpeed = 50.0;
        float movementSpeed = 10;
        float zoomSpeed = 0.03f;
        void HandleAnimationViewerInput();
        void HandleOtherInputs();

        // Target Gobj
        IHCEngine::Core::GameObject* movingGobj = nullptr;
        IHCEngine::Component::AnimatorComponent* animator = nullptr;

        // Data Control points 
        std::vector<glm::vec3> data;
        int dataSetIndex = 0;
        void changeNextDataSet();

        // Control point visualization
        std::vector<IHCEngine::Core::GameObject*> debugControlPoints;
        void createDebugControlPoints();
        int controlPointID= 0;

        // Path logic
        IHCEngine::Math::SpaceCurve spaceCurve;

        // Path visualization
        IHCEngine::Component::LineRendererComponent* lineRenderer;

        // Ease in/out speed control
        IHCEngine::Math::SpeedControl speedControl;
        float easeInTiming = 0;
        float easeOutTiming = 0;
        float totalTime = 0;

        // Pace Control (Sliding and skidding)
        IHCEngine::Math::PaceControl paceControl;

        // OrientationControl
        IHCEngine::Math::OrientationControl orientationControl;

        // Motion blending
        IHCEngine::Graphics::BlendTree blendTree;
        float blendRatio = 0.0;
        float upperBoundSpeed = 0.0;
        float lowerBoundSpeed = 0.0;
    };
}
