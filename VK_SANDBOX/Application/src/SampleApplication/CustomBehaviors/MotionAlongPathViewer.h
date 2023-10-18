#pragma once
#include <GLFW/glfw3.h>
#include "../../Engine/src/Core/Scene/Components/CustomBehavior/CustomBehavior.h"
#include "../../../../Engine/src/Math/Path/SpaceCurve.h"
#include "../../../../Engine/src/Math/Path/SpeedControl.h"

namespace IHCEngine::Component
{
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


    private:

        // Data 
        std::vector<glm::vec3> data;

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
        
        //test
        IHCEngine::Core::GameObject* testMoveGobj = nullptr;
        IHCEngine::Core::GameObject* testMoveGobj2 = nullptr;

        bool startMove = false;
        float passedTime = 0;

        // GLFWwindow* window = nullptr;
        //IHCEngine::Graphics::Camera* camera = nullptr;

    };
}
