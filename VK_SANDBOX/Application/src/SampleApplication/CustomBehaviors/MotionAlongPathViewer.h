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

        std::vector<glm::vec3>& GetControlPoints() { return data; }
        ArcLengthTable& GetArcLengthTable();
        float GetCurrentSpeed() { return currentSpeed; }
        float GetTimePassed() { return passedTime; }

    private:

        bool isMoving = false;
        float totalTime = 0;
        float currentSpeed = 0;
        float prevFrameDistance = 0;
        float passedTime = 0;

        // Data points
        std::vector<glm::vec3> data;
        int dataSetIndex = 0;
        void changeNextDataSet();

        // Control point visualization
        std::vector<IHCEngine::Core::GameObject*> debugControlPoints;
        void createDebugControlPoints();
        int controlPointID= 0;

        // Path visualization
        IHCEngine::Component::LineRendererComponent* lineRenderer;

        // Path logic
        IHCEngine::Math::SpaceCurve spaceCurve;

        // Ease in/out speed control
        IHCEngine::Math::SpeedControl speedControl;

        // Pace Control (Sliding and skidding)
        IHCEngine::Math::PaceControl paceControl;

        // OrientationControl
        IHCEngine::Math::OrientationControl orientationControl;

        // Motion blending
        IHCEngine::Graphics::BlendTree blendTree;

        //test
        IHCEngine::Component::AnimatorComponent* animator;
        IHCEngine::Component::AnimatorComponent* animator2;
        IHCEngine::Core::GameObject* testMoveGobj = nullptr;
        //IHCEngine::Core::GameObject* testMoveGobj2 = nullptr;

        // GLFWwindow* window = nullptr;
        //IHCEngine::Graphics::Camera* camera = nullptr;

    };
}
